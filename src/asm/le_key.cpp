#include "asm/le_key.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

namespace asmproto {

namespace {

std::string bytes_to_hex(std::span<const std::uint8_t> bytes) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (const auto byte : bytes) {
        out << std::setw(2) << static_cast<int>(byte);
    }
    return out.str();
}

std::array<std::uint8_t, 16> hex_to_key(std::string_view hex) {
    if (hex.size() != 32U) {
        throw AsmError("LE key hex string must be 32 characters");
    }
    std::array<std::uint8_t, 16> key {};
    for (std::size_t i = 0; i < key.size(); ++i) {
        const auto pair = std::string(hex.substr(i * 2U, 2U));
        key[i] = static_cast<std::uint8_t>(std::stoul(pair, nullptr, 16));
    }
    return key;
}

void append_record(ByteBuffer& out, const LeKeyRecord& key) {
    append_be_u32(out, key.key_id);
    out.insert(out.end(), key.key.begin(), key.key.end());
    append_be_u32(out, key.expire_time);
    append_be_u64(out, key.attribute_data);
}

LeKeyRecord decode_record(std::span<const std::uint8_t> bytes, std::size_t offset) {
    if (bytes.size() < offset + 32U) {
        throw AsmError("LE key batch item truncated");
    }

    LeKeyRecord key;
    key.key_id = decode_be_u32(bytes, offset);
    offset += 4U;
    std::copy_n(bytes.begin() + static_cast<std::ptrdiff_t>(offset), 16U, key.key.begin());
    offset += 16U;
    key.expire_time = decode_be_u32(bytes, offset);
    offset += 4U;
    key.attribute_data = decode_be_u64(bytes, offset);
    return key;
}

ByteBuffer encode_u32_batch(std::span<const std::uint32_t> values) {
    ByteBuffer out;
    append_be_u32(out, static_cast<std::uint32_t>(values.size()));
    append_be_u32(out, 4U);
    for (const auto value : values) {
        append_be_u32(out, value);
    }
    return out;
}

std::vector<std::uint32_t> decode_u32_batch(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < 8U) {
        throw AsmError("LEKeyID batch too short");
    }
    const auto count = decode_be_u32(bytes, 0U);
    const auto item_length = decode_be_u32(bytes, 4U);
    if (item_length != 4U) {
        throw AsmError("LEKeyID batch item length must be 4");
    }
    if (bytes.size() != 8U + static_cast<std::size_t>(count) * 4U) {
        throw AsmError("LEKeyID batch size mismatch");
    }

    std::vector<std::uint32_t> key_ids;
    key_ids.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        key_ids.push_back(decode_be_u32(bytes, 8U + i * 4U));
    }
    return key_ids;
}

bool would_overflow(std::span<const LeKeyRecord> incoming, std::span<const LeKeyRecord> existing, std::size_t capacity) {
    std::size_t unique_count = existing.size();
    for (const auto& key : incoming) {
        const auto found = std::find_if(existing.begin(), existing.end(), [&](const LeKeyRecord& current) {
            return current.key_id == key.key_id;
        });
        if (found == existing.end()) {
            ++unique_count;
        }
    }
    return unique_count > capacity;
}

LeKeyRecord parse_jsonl_line(const std::string& line) {
    static const std::regex pattern(
        R"json(^\s*\{\s*"key_id"\s*:\s*([0-9]+)\s*,\s*"key"\s*:\s*"([0-9A-Fa-f]{32})"\s*,\s*"expire_time"\s*:\s*([0-9]+)\s*,\s*"attribute_data"\s*:\s*([0-9]+)\s*\}\s*$)json");
    std::smatch match;
    if (!std::regex_match(line, match, pattern)) {
        throw AsmError("invalid LEKey JSONL line format");
    }

    return {
        .key_id = static_cast<std::uint32_t>(std::stoul(match[1].str())),
        .key = hex_to_key(match[2].str()),
        .expire_time = static_cast<std::uint32_t>(std::stoul(match[3].str())),
        .attribute_data = static_cast<std::uint64_t>(std::stoull(match[4].str())),
    };
}

}  // namespace

bool InMemoryLeKeyStore::load_keys(std::span<const LeKeyRecord> keys) {
    if (would_overflow(keys, keys_, kCapacity)) {
        return true;
    }

    for (const auto& incoming : keys) {
        const auto found = std::find_if(keys_.begin(), keys_.end(), [&](const LeKeyRecord& current) {
            return current.key_id == incoming.key_id;
        });
        if (found != keys_.end()) {
            *found = incoming;
        } else {
            keys_.push_back(incoming);
        }
    }
    return false;
}

bool InMemoryLeKeyStore::contains_key(std::uint32_t key_id) const {
    return std::any_of(keys_.begin(), keys_.end(), [&](const LeKeyRecord& key) {
        return key.key_id == key_id;
    });
}

std::vector<std::uint32_t> InMemoryLeKeyStore::list_key_ids() const {
    std::vector<std::uint32_t> key_ids;
    key_ids.reserve(keys_.size());
    for (const auto& key : keys_) {
        key_ids.push_back(key.key_id);
    }
    return key_ids;
}

bool InMemoryLeKeyStore::purge_key(std::uint32_t key_id) {
    const auto before = keys_.size();
    std::erase_if(keys_, [&](const LeKeyRecord& key) {
        return key.key_id == key_id;
    });
    return keys_.size() != before;
}

void InMemoryLeKeyStore::purge_all() {
    keys_.clear();
}

JsonlLeKeyStore::JsonlLeKeyStore(const std::filesystem::path& path)
    : path_(path) {
    if (!std::filesystem::exists(path_)) {
        std::ofstream create(path_);
        if (!create.is_open()) {
            throw AsmError("failed to create LEKey JSONL store");
        }
        return;
    }

    std::ifstream input(path_);
    if (!input.is_open()) {
        throw AsmError("failed to open LEKey JSONL store");
    }

    std::string line;
    std::size_t line_number = 0U;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) {
            continue;
        }
        try {
            keys_.push_back(parse_jsonl_line(line));
        } catch (const std::exception& ex) {
            throw AsmError("failed to parse LEKey JSONL line " + std::to_string(line_number) + ": " + ex.what());
        }
    }
}

bool JsonlLeKeyStore::load_keys(std::span<const LeKeyRecord> keys) {
    if (would_overflow(keys, keys_, kCapacity)) {
        return true;
    }

    for (const auto& incoming : keys) {
        const auto found = std::find_if(keys_.begin(), keys_.end(), [&](const LeKeyRecord& current) {
            return current.key_id == incoming.key_id;
        });
        if (found != keys_.end()) {
            *found = incoming;
        } else {
            keys_.push_back(incoming);
        }
    }
    persist();
    return false;
}

bool JsonlLeKeyStore::contains_key(std::uint32_t key_id) const {
    return std::any_of(keys_.begin(), keys_.end(), [&](const LeKeyRecord& key) {
        return key.key_id == key_id;
    });
}

std::vector<std::uint32_t> JsonlLeKeyStore::list_key_ids() const {
    std::vector<std::uint32_t> key_ids;
    key_ids.reserve(keys_.size());
    for (const auto& key : keys_) {
        key_ids.push_back(key.key_id);
    }
    return key_ids;
}

bool JsonlLeKeyStore::purge_key(std::uint32_t key_id) {
    const auto before = keys_.size();
    std::erase_if(keys_, [&](const LeKeyRecord& key) {
        return key.key_id == key_id;
    });
    const bool removed = keys_.size() != before;
    if (removed) {
        persist();
    }
    return removed;
}

void JsonlLeKeyStore::purge_all() {
    keys_.clear();
    persist();
}

void JsonlLeKeyStore::persist() const {
    std::ofstream output(path_, std::ios::trunc);
    if (!output.is_open()) {
        throw AsmError("failed to persist LEKey JSONL store");
    }

    for (const auto& key : keys_) {
        output << "{\"key_id\":" << key.key_id
               << ",\"key\":\"" << bytes_to_hex(key.key)
               << "\",\"expire_time\":" << key.expire_time
               << ",\"attribute_data\":" << key.attribute_data << "}\n";
    }
}

ByteBuffer LeKeyPayloadCodec::encode_load_request(const LeKeyLoadRequestPayload& payload) {
    ByteBuffer out;
    append_be_u32(out, static_cast<std::uint32_t>(payload.keys.size()));
    append_be_u32(out, 32U);
    for (const auto& key : payload.keys) {
        append_record(out, key);
    }
    return out;
}

LeKeyLoadRequestPayload LeKeyPayloadCodec::decode_load_request(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < 8U) {
        throw AsmError("LEKeyLoad payload too short");
    }
    const auto count = decode_be_u32(bytes, 0U);
    const auto item_length = decode_be_u32(bytes, 4U);
    if (item_length != 32U) {
        throw AsmError("LEKeyLoad item length must be 32");
    }
    if (bytes.size() != 8U + static_cast<std::size_t>(count) * 32U) {
        throw AsmError("LEKeyLoad payload size mismatch");
    }

    LeKeyLoadRequestPayload payload;
    payload.keys.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        payload.keys.push_back(decode_record(bytes, 8U + i * 32U));
    }
    return payload;
}

ByteBuffer LeKeyPayloadCodec::encode_load_response(const LeKeyLoadResponsePayload& payload) {
    return ByteBuffer {static_cast<std::uint8_t>(payload.overflow ? 1U : 0U)};
}

LeKeyLoadResponsePayload LeKeyPayloadCodec::decode_load_response(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 1U) {
        throw AsmError("LEKeyLoad response payload must be 1 byte");
    }
    return {
        .overflow = bytes[0] != 0U,
    };
}

ByteBuffer LeKeyPayloadCodec::encode_query_id_request(const LeKeyQueryIdRequestPayload& payload) {
    ByteBuffer out;
    append_be_u32(out, payload.key_id);
    return out;
}

LeKeyQueryIdRequestPayload LeKeyPayloadCodec::decode_query_id_request(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 4U) {
        throw AsmError("LEKeyQueryID request payload must be 4 bytes");
    }
    return {
        .key_id = decode_be_u32(bytes, 0U),
    };
}

ByteBuffer LeKeyPayloadCodec::encode_query_id_response(const LeKeyQueryIdResponsePayload& payload) {
    return ByteBuffer {static_cast<std::uint8_t>(payload.key_present ? 1U : 0U)};
}

LeKeyQueryIdResponsePayload LeKeyPayloadCodec::decode_query_id_response(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 1U) {
        throw AsmError("LEKeyQueryID response payload must be 1 byte");
    }
    return {
        .key_present = bytes[0] != 0U,
    };
}

ByteBuffer LeKeyPayloadCodec::encode_query_all_response(const LeKeyQueryAllResponsePayload& payload) {
    return encode_u32_batch(payload.key_ids);
}

LeKeyQueryAllResponsePayload LeKeyPayloadCodec::decode_query_all_response(std::span<const std::uint8_t> bytes) {
    return {
        .key_ids = decode_u32_batch(bytes),
    };
}

ByteBuffer LeKeyPayloadCodec::encode_purge_id_request(const LeKeyPurgeIdRequestPayload& payload) {
    ByteBuffer out;
    append_be_u32(out, payload.key_id);
    return out;
}

LeKeyPurgeIdRequestPayload LeKeyPayloadCodec::decode_purge_id_request(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 4U) {
        throw AsmError("LEKeyPurgeID request payload must be 4 bytes");
    }
    return {
        .key_id = decode_be_u32(bytes, 0U),
    };
}

ByteBuffer LeKeyPayloadCodec::encode_purge_id_response(const LeKeyPurgeIdResponsePayload& payload) {
    return ByteBuffer {static_cast<std::uint8_t>(payload.no_key_id ? 1U : 0U)};
}

LeKeyPurgeIdResponsePayload LeKeyPayloadCodec::decode_purge_id_response(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 1U) {
        throw AsmError("LEKeyPurgeID response payload must be 1 byte");
    }
    return {
        .no_key_id = bytes[0] != 0U,
    };
}

}  // namespace asmproto

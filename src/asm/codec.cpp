#include "asm/codec.hpp"

#include <array>

namespace asmproto {

namespace {

struct UlMapEntry {
    Command command;
    bool response;
    Ul ul;
};

constexpr Ul make_ul(std::uint8_t byte12, std::uint8_t byte13) {
    return Ul {
        0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01,
        0x02, 0x07, 0x01, byte12, byte13, 0x00, 0x00, 0x00,
    };
}

constexpr std::array<UlMapEntry, 19> kUlMap {{
    {Command::bad_request, true, make_ul(0x01, 0x01)},
    {Command::get_time, false, make_ul(0x02, 0x10)},
    {Command::get_time, true, make_ul(0x02, 0x11)},
    {Command::get_event_list, false, make_ul(0x02, 0x12)},
    {Command::get_event_list, true, make_ul(0x02, 0x13)},
    {Command::get_event_id, false, make_ul(0x02, 0x14)},
    {Command::get_event_id, true, make_ul(0x02, 0x15)},
    {Command::query_spb, false, make_ul(0x02, 0x16)},
    {Command::query_spb, true, make_ul(0x02, 0x17)},
    {Command::le_key_load, false, make_ul(0x03, 0x20)},
    {Command::le_key_load, true, make_ul(0x03, 0x21)},
    {Command::le_key_query_id, false, make_ul(0x03, 0x22)},
    {Command::le_key_query_id, true, make_ul(0x03, 0x23)},
    {Command::le_key_query_all, false, make_ul(0x03, 0x24)},
    {Command::le_key_query_all, true, make_ul(0x03, 0x25)},
    {Command::le_key_purge_id, false, make_ul(0x03, 0x26)},
    {Command::le_key_purge_id, true, make_ul(0x03, 0x27)},
    {Command::le_key_purge_all, false, make_ul(0x03, 0x28)},
    {Command::le_key_purge_all, true, make_ul(0x03, 0x29)},
}};

const UlMapEntry& lookup_entry(Command command, bool response) {
    for (const auto& entry : kUlMap) {
        if (entry.command == command && entry.response == response) {
            return entry;
        }
    }
    throw AsmError("missing UL mapping");
}

GeneralResponse decode_response_code(std::uint8_t value) {
    switch (value) {
        case 0x00:
            return GeneralResponse::successful;
        case 0x01:
            return GeneralResponse::failed;
        case 0x02:
            return GeneralResponse::invalid;
        case 0x03:
            return GeneralResponse::busy;
        default:
            throw AsmError("invalid general response code");
    }
}

}  // namespace

std::uint32_t decode_be_u32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    if (bytes.size() < offset + 4U) {
        throw AsmError("unexpected end of buffer while reading u32");
    }

    return (static_cast<std::uint32_t>(bytes[offset]) << 24U)
        | (static_cast<std::uint32_t>(bytes[offset + 1U]) << 16U)
        | (static_cast<std::uint32_t>(bytes[offset + 2U]) << 8U)
        | static_cast<std::uint32_t>(bytes[offset + 3U]);
}

std::uint64_t decode_be_u64(std::span<const std::uint8_t> bytes, std::size_t offset) {
    if (bytes.size() < offset + 8U) {
        throw AsmError("unexpected end of buffer while reading u64");
    }

    std::uint64_t value = 0U;
    for (std::size_t i = 0; i < 8U; ++i) {
        value = (value << 8U) | bytes[offset + i];
    }
    return value;
}

void append_be_u32(ByteBuffer& out, std::uint32_t value) {
    out.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xffU));
    out.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xffU));
    out.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
    out.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void append_be_u64(ByteBuffer& out, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        out.push_back(static_cast<std::uint8_t>((value >> shift) & 0xffU));
    }
}

const char* to_string(Command command) {
    switch (command) {
        case Command::bad_request:
            return "BadRequest";
        case Command::get_time:
            return "GetTime";
        case Command::get_event_list:
            return "GetEventList";
        case Command::get_event_id:
            return "GetEventID";
        case Command::query_spb:
            return "QuerySPB";
        case Command::le_key_load:
            return "LEKeyLoad";
        case Command::le_key_query_id:
            return "LEKeyQueryID";
        case Command::le_key_query_all:
            return "LEKeyQueryAll";
        case Command::le_key_purge_id:
            return "LEKeyPurgeID";
        case Command::le_key_purge_all:
            return "LEKeyPurgeAll";
        default:
            return "Unknown";
    }
}

const char* to_string(GeneralResponse response) {
    switch (response) {
        case GeneralResponse::successful:
            return "successful";
        case GeneralResponse::failed:
            return "failed";
        case GeneralResponse::invalid:
            return "invalid";
        case GeneralResponse::busy:
            return "busy";
        default:
            return "unknown";
    }
}

KlvPacket Codec::encode_request(const StandardRequest& request) {
    KlvPacket packet;
    packet.ul = ul_for(request.command, false);
    append_be_u32(packet.value, request.request_id);
    packet.value.insert(packet.value.end(), request.payload.begin(), request.payload.end());
    packet.length = static_cast<std::uint32_t>(packet.value.size());
    return packet;
}

StandardRequest Codec::decode_request(const KlvPacket& packet) {
    const auto descriptor = describe_ul(packet.ul);
    if (descriptor.response) {
        throw AsmError("request packet expected");
    }
    if (packet.value.size() < 4U) {
        throw AsmError("request payload too short for request id");
    }

    StandardRequest request;
    request.command = descriptor.command;
    request.request_id = decode_be_u32(packet.value, 0U);
    request.payload.assign(packet.value.begin() + 4, packet.value.end());
    return request;
}

KlvPacket Codec::encode_response(const StandardResponse& response) {
    KlvPacket packet;
    packet.ul = ul_for(response.command, true);
    if (response.command != Command::bad_request) {
        if (!response.request_id.has_value()) {
            throw AsmError("response request id missing");
        }
        append_be_u32(packet.value, *response.request_id);
    }
    packet.value.insert(packet.value.end(), response.payload.begin(), response.payload.end());
    packet.value.push_back(static_cast<std::uint8_t>(response.response_code));
    packet.length = static_cast<std::uint32_t>(packet.value.size());
    return packet;
}

StandardResponse Codec::decode_response(const KlvPacket& packet) {
    const auto descriptor = describe_ul(packet.ul);
    if (!descriptor.response) {
        throw AsmError("response packet expected");
    }
    if (packet.value.empty()) {
        throw AsmError("response payload too short");
    }

    StandardResponse response;
    response.command = descriptor.command;
    response.response_code = decode_response_code(packet.value.back());

    if (descriptor.command == Command::bad_request) {
        response.payload.assign(packet.value.begin(), packet.value.end() - 1);
        return response;
    }

    if (packet.value.size() < 5U) {
        throw AsmError("response payload too short for request id");
    }
    response.request_id = decode_be_u32(packet.value, 0U);
    response.payload.assign(packet.value.begin() + 4, packet.value.end() - 1);
    return response;
}

CommandDescriptor Codec::describe_ul(const Ul& ul) {
    for (const auto& entry : kUlMap) {
        if (entry.ul == ul) {
            return {
                .command = entry.command,
                .response = entry.response,
            };
        }
    }
    throw AsmError("unknown ASM UL");
}

const Ul& Codec::ul_for(Command command, bool response) {
    return lookup_entry(command, response).ul;
}

}  // namespace asmproto

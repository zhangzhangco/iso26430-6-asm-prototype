#include "asm/event_log.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

namespace asmproto {

namespace {

std::uint32_t parse_utc_timestamp(std::string_view text) {
    std::tm tm {};
    std::istringstream input {std::string(text)};
    input >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (input.fail()) {
        throw AsmError("invalid timestamp format in event log");
    }
#if defined(_WIN32)
    const auto epoch = _mkgmtime(&tm);
#else
    const auto epoch = timegm(&tm);
#endif
    if (epoch < 0) {
        throw AsmError("timestamp conversion failed");
    }
    return static_cast<std::uint32_t>(epoch);
}

EventSeverity severity_from_string(std::string_view value) {
    if (value == "info") {
        return EventSeverity::info;
    }
    if (value == "warning") {
        return EventSeverity::warning;
    }
    if (value == "error") {
        return EventSeverity::error;
    }
    throw AsmError("unsupported event severity in JSONL");
}

std::string escape_xml(std::string_view value) {
    std::string out;
    out.reserve(value.size());
    for (const auto ch : value) {
        switch (ch) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\'':
                out += "&apos;";
                break;
            default:
                out.push_back(ch);
                break;
        }
    }
    return out;
}

std::string render_event_xml(const EventRecord& event) {
    std::ostringstream out;
    out << "<LogRecord>"
        << "<EventID>" << event.event_id << "</EventID>"
        << "<Timestamp>" << escape_xml(event.timestamp_text) << "</Timestamp>"
        << "<Severity>" << escape_xml(to_string(event.severity)) << "</Severity>"
        << "<Summary>" << escape_xml(event.summary) << "</Summary>"
        << "</LogRecord>";
    return out.str();
}

EventRecord parse_jsonl_line(const std::string& line) {
    static const std::regex pattern(
        R"json(^\s*\{\s*"event_id"\s*:\s*([0-9]+)\s*,\s*"severity"\s*:\s*"([^"]+)"\s*,\s*"timestamp"\s*:\s*"([^"]+)"\s*,\s*"summary"\s*:\s*"([^"]*)"\s*\}\s*$)json");

    std::smatch match;
    if (!std::regex_match(line, match, pattern)) {
        throw AsmError("invalid event JSONL line format");
    }

    const auto timestamp = match[3].str();
    return EventRecord {
        .event_id = static_cast<std::uint32_t>(std::stoul(match[1].str())),
        .severity = severity_from_string(match[2].str()),
        .timestamp_epoch = parse_utc_timestamp(timestamp),
        .timestamp_text = timestamp,
        .summary = match[4].str(),
    };
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
        throw AsmError("batch payload too short");
    }
    const auto count = decode_be_u32(bytes, 0U);
    const auto item_length = decode_be_u32(bytes, 4U);
    if (item_length != 4U) {
        throw AsmError("unexpected batch item length");
    }
    if (bytes.size() != 8U + static_cast<std::size_t>(count) * 4U) {
        throw AsmError("batch payload size mismatch");
    }

    std::vector<std::uint32_t> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        out.push_back(decode_be_u32(bytes, 8U + i * 4U));
    }
    return out;
}

}  // namespace

InMemoryEventLogProvider::InMemoryEventLogProvider()
    : events_({
          EventRecord {
              .event_id = 1001U,
              .severity = EventSeverity::info,
              .timestamp_epoch = 1776758400U,
              .timestamp_text = "2026-04-21T08:00:00Z",
              .summary = "Responder started",
          },
          EventRecord {
              .event_id = 1002U,
              .severity = EventSeverity::warning,
              .timestamp_epoch = 1776758700U,
              .timestamp_text = "2026-04-21T08:05:00Z",
              .summary = "TLS profile uses development CA",
          },
          EventRecord {
              .event_id = 1003U,
              .severity = EventSeverity::error,
              .timestamp_epoch = 1776759000U,
              .timestamp_text = "2026-04-21T08:10:00Z",
              .summary = "Link encryption material not loaded",
          },
      }) {}

InMemoryEventLogProvider::InMemoryEventLogProvider(std::vector<EventRecord> events)
    : events_(std::move(events)) {}

std::vector<std::uint32_t> InMemoryEventLogProvider::list_event_ids(std::uint32_t time_start, std::uint32_t time_stop) const {
    std::vector<std::uint32_t> event_ids;
    for (const auto& event : events_) {
        if (event.timestamp_epoch >= time_start && event.timestamp_epoch <= time_stop) {
            event_ids.push_back(event.event_id);
        }
    }
    return event_ids;
}

std::optional<std::string> InMemoryEventLogProvider::get_event_xml(std::uint32_t event_id) const {
    for (const auto& event : events_) {
        if (event.event_id == event_id) {
            return render_event_xml(event);
        }
    }
    return std::nullopt;
}

JsonlEventLogProvider::JsonlEventLogProvider(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw AsmError("failed to open JSONL event log file");
    }

    std::string line;
    std::size_t line_number = 0U;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) {
            continue;
        }
        try {
            events_.push_back(parse_jsonl_line(line));
        } catch (const std::exception& ex) {
            throw AsmError("failed to parse event JSONL line " + std::to_string(line_number) + ": " + ex.what());
        }
    }
}

std::vector<std::uint32_t> JsonlEventLogProvider::list_event_ids(std::uint32_t time_start, std::uint32_t time_stop) const {
    std::vector<std::uint32_t> event_ids;
    for (const auto& event : events_) {
        if (event.timestamp_epoch >= time_start && event.timestamp_epoch <= time_stop) {
            event_ids.push_back(event.event_id);
        }
    }
    return event_ids;
}

std::optional<std::string> JsonlEventLogProvider::get_event_xml(std::uint32_t event_id) const {
    for (const auto& event : events_) {
        if (event.event_id == event_id) {
            return render_event_xml(event);
        }
    }
    return std::nullopt;
}

ByteBuffer GetEventListPayloadCodec::encode_request(const GetEventListRequestPayload& payload) {
    ByteBuffer out;
    append_be_u32(out, payload.time_start);
    append_be_u32(out, payload.time_stop);
    return out;
}

GetEventListRequestPayload GetEventListPayloadCodec::decode_request(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 8U) {
        throw AsmError("GetEventList request payload must be 8 bytes");
    }
    return {
        .time_start = decode_be_u32(bytes, 0U),
        .time_stop = decode_be_u32(bytes, 4U),
    };
}

ByteBuffer GetEventListPayloadCodec::encode_response(const GetEventListResponsePayload& payload) {
    return encode_u32_batch(payload.event_ids);
}

GetEventListResponsePayload GetEventListPayloadCodec::decode_response(std::span<const std::uint8_t> bytes) {
    return {
        .event_ids = decode_u32_batch(bytes),
    };
}

ByteBuffer GetEventIdPayloadCodec::encode_request(const GetEventIdRequestPayload& payload) {
    ByteBuffer out;
    append_be_u32(out, payload.event_id);
    return out;
}

GetEventIdRequestPayload GetEventIdPayloadCodec::decode_request(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 4U) {
        throw AsmError("GetEventID request payload must be 4 bytes");
    }
    return {
        .event_id = decode_be_u32(bytes, 0U),
    };
}

ByteBuffer GetEventIdPayloadCodec::encode_response(const GetEventIdResponsePayload& payload) {
    return ByteBuffer(payload.log_record_xml.begin(), payload.log_record_xml.end());
}

GetEventIdResponsePayload GetEventIdPayloadCodec::decode_response(std::span<const std::uint8_t> bytes) {
    return {
        .log_record_xml = std::string(bytes.begin(), bytes.end()),
    };
}

const char* to_string(EventSeverity severity) {
    switch (severity) {
        case EventSeverity::info:
            return "info";
        case EventSeverity::warning:
            return "warning";
        case EventSeverity::error:
            return "error";
        default:
            return "unknown";
    }
}

}  // namespace asmproto

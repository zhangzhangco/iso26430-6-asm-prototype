#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "asm/common.hpp"

namespace asmproto {

enum class EventSeverity : std::uint8_t {
    info = 0x00,
    warning = 0x01,
    error = 0x02,
};

struct EventRecord {
    std::uint32_t event_id {0};
    EventSeverity severity {EventSeverity::info};
    std::uint32_t timestamp_epoch {0};
    std::string timestamp_text;
    std::string summary;
};

struct GetEventListRequestPayload {
    std::uint32_t time_start {0};
    std::uint32_t time_stop {0};
};

struct GetEventListResponsePayload {
    std::vector<std::uint32_t> event_ids;
};

struct GetEventIdRequestPayload {
    std::uint32_t event_id {0};
};

struct GetEventIdResponsePayload {
    std::string log_record_xml;
};

class EventLogStore {
public:
    virtual ~EventLogStore() = default;
    virtual std::vector<std::uint32_t> list_event_ids(std::uint32_t time_start, std::uint32_t time_stop) const = 0;
    virtual std::optional<std::string> get_event_xml(std::uint32_t event_id) const = 0;
};

class InMemoryEventLogProvider final : public EventLogStore {
public:
    InMemoryEventLogProvider();
    explicit InMemoryEventLogProvider(std::vector<EventRecord> events);

    std::vector<std::uint32_t> list_event_ids(std::uint32_t time_start, std::uint32_t time_stop) const override;
    std::optional<std::string> get_event_xml(std::uint32_t event_id) const override;

private:
    std::vector<EventRecord> events_;
};

class JsonlEventLogProvider final : public EventLogStore {
public:
    explicit JsonlEventLogProvider(const std::filesystem::path& path);

    std::vector<std::uint32_t> list_event_ids(std::uint32_t time_start, std::uint32_t time_stop) const override;
    std::optional<std::string> get_event_xml(std::uint32_t event_id) const override;

private:
    std::vector<EventRecord> events_;
};

class GetEventListPayloadCodec {
public:
    static ByteBuffer encode_request(const GetEventListRequestPayload& payload);
    static GetEventListRequestPayload decode_request(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_response(const GetEventListResponsePayload& payload);
    static GetEventListResponsePayload decode_response(std::span<const std::uint8_t> bytes);
};

class GetEventIdPayloadCodec {
public:
    static ByteBuffer encode_request(const GetEventIdRequestPayload& payload);
    static GetEventIdRequestPayload decode_request(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_response(const GetEventIdResponsePayload& payload);
    static GetEventIdResponsePayload decode_response(std::span<const std::uint8_t> bytes);
};

const char* to_string(EventSeverity severity);

}  // namespace asmproto

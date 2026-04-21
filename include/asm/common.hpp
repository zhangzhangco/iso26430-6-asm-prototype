#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace asmproto {

using ByteBuffer = std::vector<std::uint8_t>;
using RequestId = std::uint32_t;
using Ul = std::array<std::uint8_t, 16>;

class AsmError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

enum class Command : std::uint8_t {
    bad_request = 0x00,
    get_time = 0x01,
    get_event_list = 0x02,
    get_event_id = 0x03,
    query_spb = 0x04,
    le_key_load = 0x10,
    le_key_query_id = 0x11,
    le_key_query_all = 0x12,
    le_key_purge_id = 0x13,
    le_key_purge_all = 0x14,
};

enum class GeneralResponse : std::uint8_t {
    successful = 0x00,
    failed = 0x01,
    invalid = 0x02,
    busy = 0x03,
};

struct StandardRequest {
    Command command {Command::get_time};
    RequestId request_id {0};
    ByteBuffer payload;
};

struct StandardResponse {
    Command command {Command::bad_request};
    std::optional<RequestId> request_id;
    ByteBuffer payload;
    GeneralResponse response_code {GeneralResponse::successful};
};

struct PeerIdentity {
    std::string subject;
    std::string issuer;
    std::string sha256_thumbprint;
};

struct RequestContext {
    std::optional<PeerIdentity> peer;
    std::chrono::system_clock::time_point received_at;
};

struct ServiceResult {
    StandardResponse response;
    std::string note;
};

std::uint32_t decode_be_u32(std::span<const std::uint8_t> bytes, std::size_t offset);
std::uint64_t decode_be_u64(std::span<const std::uint8_t> bytes, std::size_t offset);
void append_be_u32(ByteBuffer& out, std::uint32_t value);
void append_be_u64(ByteBuffer& out, std::uint64_t value);

const char* to_string(Command command);
const char* to_string(GeneralResponse response);

}  // namespace asmproto

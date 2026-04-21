#pragma once

#include "asm/common.hpp"

namespace asmproto {

enum class QuerySpbStatus : std::uint8_t {
    not_playing = 0x00,
    playing = 0x01,
    security_alert = 0x02,
};

struct QuerySpbResponsePayload {
    std::uint8_t protocol_version {0x01};
    QuerySpbStatus status {QuerySpbStatus::not_playing};
};

class QuerySpbPayloadCodec {
public:
    static ByteBuffer encode_response(const QuerySpbResponsePayload& payload);
    static QuerySpbResponsePayload decode_response(std::span<const std::uint8_t> bytes);
};

class QuerySpbStatusProvider {
public:
    virtual ~QuerySpbStatusProvider() = default;
    virtual QuerySpbStatus current_status() const = 0;
};

class StaticQuerySpbStatusProvider final : public QuerySpbStatusProvider {
public:
    explicit StaticQuerySpbStatusProvider(QuerySpbStatus status = QuerySpbStatus::not_playing);
    QuerySpbStatus current_status() const override;

private:
    QuerySpbStatus status_;
};

const char* to_string(QuerySpbStatus status);

}  // namespace asmproto

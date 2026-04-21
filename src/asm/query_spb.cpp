#include "asm/query_spb.hpp"

namespace asmproto {

ByteBuffer QuerySpbPayloadCodec::encode_response(const QuerySpbResponsePayload& payload) {
    return ByteBuffer {
        payload.protocol_version,
        static_cast<std::uint8_t>(payload.status),
    };
}

QuerySpbResponsePayload QuerySpbPayloadCodec::decode_response(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != 2U) {
        throw AsmError("QuerySPB response payload must be 2 bytes");
    }
    return {
        .protocol_version = bytes[0],
        .status = static_cast<QuerySpbStatus>(bytes[1]),
    };
}

StaticQuerySpbStatusProvider::StaticQuerySpbStatusProvider(QuerySpbStatus status)
    : status_(status) {}

QuerySpbStatus StaticQuerySpbStatusProvider::current_status() const {
    return status_;
}

const char* to_string(QuerySpbStatus status) {
    switch (status) {
        case QuerySpbStatus::not_playing:
            return "not_playing";
        case QuerySpbStatus::playing:
            return "playing";
        case QuerySpbStatus::security_alert:
            return "security_alert";
        default:
            return "unknown";
    }
}

}  // namespace asmproto

#include "asm/codec.hpp"
#include "asm/event_log.hpp"
#include "asm/le_key.hpp"
#include "asm/query_spb.hpp"

#include <iomanip>
#include <sstream>

#include "klv/klv.hpp"
#include "testkit.hpp"

namespace {

std::string to_hex(std::span<const std::uint8_t> bytes) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (const auto byte : bytes) {
        out << std::setw(2) << static_cast<int>(byte);
    }
    return out.str();
}

asmproto::LeKeyRecord make_key(std::uint32_t key_id) {
    return asmproto::LeKeyRecord {
        .key_id = key_id,
        .key = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
        .expire_time = 3600U,
        .attribute_data = 123456789ULL,
    };
}

}  // namespace

TEST_CASE(codec_roundtrip_preserves_request_fields) {
    asmproto::StandardRequest input;
    input.command = asmproto::Command::get_event_id;
    input.request_id = 0x01020304U;
    input.payload = asmproto::GetEventIdPayloadCodec::encode_request({
        .event_id = 1002U,
    });

    const auto packet = asmproto::Codec::encode_request(input);
    const auto bytes = asmproto::Klv::encode(packet);
    const auto decoded_packet = asmproto::Klv::decode(bytes);
    const auto output = asmproto::Codec::decode_request(decoded_packet);

    REQUIRE(output.command == input.command);
    REQUIRE(output.request_id == input.request_id);
    REQUIRE(output.payload == input.payload);
}

TEST_CASE(codec_roundtrip_preserves_response_fields) {
    asmproto::StandardResponse input;
    input.command = asmproto::Command::query_spb;
    input.request_id = 0x0000000aU;
    input.payload = asmproto::QuerySpbPayloadCodec::encode_response({
        .protocol_version = 0x01,
        .status = asmproto::QuerySpbStatus::not_playing,
    });
    input.response_code = asmproto::GeneralResponse::successful;

    const auto packet = asmproto::Codec::encode_response(input);
    const auto bytes = asmproto::Klv::encode(packet);
    const auto decoded_packet = asmproto::Klv::decode(bytes);
    const auto output = asmproto::Codec::decode_response(decoded_packet);

    REQUIRE(output.command == input.command);
    REQUIRE(output.request_id == input.request_id);
    REQUIRE(output.payload == input.payload);
    REQUIRE(output.response_code == input.response_code);
}

TEST_CASE(ber_length_uses_fixed_four_byte_form) {
    const auto encoded = asmproto::Klv::encode_ber_length(0x01ffU);
    REQUIRE(encoded.size() == 4U);
    REQUIRE(encoded[0] == 0x83U);
    REQUIRE(encoded[1] == 0x00U);
    REQUIRE(encoded[2] == 0x01U);
    REQUIRE(encoded[3] == 0xffU);

    const auto [length, consumed] = asmproto::Klv::decode_ber_length(encoded);
    REQUIRE(length == 0x01ffU);
    REQUIRE(consumed == 4U);
}

TEST_CASE(codec_rejects_wrong_ul) {
    asmproto::KlvPacket packet;
    packet.ul.fill(0x00U);
    packet.value = {0x00U, 0x00U, 0x00U, 0x01U};
    packet.length = static_cast<std::uint32_t>(packet.value.size());

    bool thrown = false;
    try {
        static_cast<void>(asmproto::Codec::decode_request(packet));
    } catch (const asmproto::AsmError&) {
        thrown = true;
    }
    REQUIRE(thrown);
}

TEST_CASE(query_spb_payload_roundtrip_preserves_fields) {
    const asmproto::QuerySpbResponsePayload input {
        .protocol_version = 0x01,
        .status = asmproto::QuerySpbStatus::security_alert,
    };

    const auto encoded = asmproto::QuerySpbPayloadCodec::encode_response(input);
    const auto decoded = asmproto::QuerySpbPayloadCodec::decode_response(encoded);

    REQUIRE(decoded.protocol_version == input.protocol_version);
    REQUIRE(decoded.status == input.status);
}

TEST_CASE(event_log_payload_roundtrip_preserves_fields) {
    const auto list_request_encoded = asmproto::GetEventListPayloadCodec::encode_request({
        .time_start = 1776758400U,
        .time_stop = 1776759000U,
    });
    const auto list_request_decoded = asmproto::GetEventListPayloadCodec::decode_request(list_request_encoded);
    REQUIRE(list_request_decoded.time_start == 1776758400U);
    REQUIRE(list_request_decoded.time_stop == 1776759000U);

    const auto list_response_encoded = asmproto::GetEventListPayloadCodec::encode_response({
        .event_ids = {1001U, 1002U, 1003U},
    });
    const auto list_response_decoded = asmproto::GetEventListPayloadCodec::decode_response(list_response_encoded);
    REQUIRE(list_response_decoded.event_ids.size() == 3U);
    REQUIRE(list_response_decoded.event_ids[1] == 1002U);

    const auto detail_request_encoded = asmproto::GetEventIdPayloadCodec::encode_request({
        .event_id = 1002U,
    });
    const auto detail_request_decoded = asmproto::GetEventIdPayloadCodec::decode_request(detail_request_encoded);
    REQUIRE(detail_request_decoded.event_id == 1002U);

    const auto detail_response_encoded = asmproto::GetEventIdPayloadCodec::encode_response({
        .log_record_xml = "<LogRecord/>",
    });
    const auto detail_response_decoded = asmproto::GetEventIdPayloadCodec::decode_response(detail_response_encoded);
    REQUIRE(detail_response_decoded.log_record_xml == "<LogRecord/>");
}

TEST_CASE(le_key_payload_roundtrip_preserves_fields) {
    const auto load_request_encoded = asmproto::LeKeyPayloadCodec::encode_load_request({
        .keys = {make_key(4001U)},
    });
    const auto load_request_decoded = asmproto::LeKeyPayloadCodec::decode_load_request(load_request_encoded);
    REQUIRE(load_request_decoded.keys.size() == 1U);
    REQUIRE(load_request_decoded.keys[0].key_id == 4001U);
    REQUIRE(load_request_decoded.keys[0].attribute_data == 123456789ULL);

    const auto load_response_encoded = asmproto::LeKeyPayloadCodec::encode_load_response({
        .overflow = true,
    });
    const auto load_response_decoded = asmproto::LeKeyPayloadCodec::decode_load_response(load_response_encoded);
    REQUIRE(load_response_decoded.overflow);

    const auto query_all_encoded = asmproto::LeKeyPayloadCodec::encode_query_all_response({
        .key_ids = {4001U, 4002U},
    });
    const auto query_all_decoded = asmproto::LeKeyPayloadCodec::decode_query_all_response(query_all_encoded);
    REQUIRE(query_all_decoded.key_ids.size() == 2U);
    REQUIRE(query_all_decoded.key_ids[1] == 4002U);
}

TEST_CASE(protocol_vector_get_time_request_frame_stable) {
    const auto frame = asmproto::Klv::encode(asmproto::Codec::encode_request({
        .command = asmproto::Command::get_time,
        .request_id = 1U,
        .payload = {},
    }));
    REQUIRE(to_hex(frame) == "060e2b340205010102070102100000008300000400000001");
}

TEST_CASE(protocol_vector_get_event_list_response_frame_stable) {
    const auto frame = asmproto::Klv::encode(asmproto::Codec::encode_response({
        .command = asmproto::Command::get_event_list,
        .request_id = 2U,
        .payload = asmproto::GetEventListPayloadCodec::encode_response({
            .event_ids = {1001U, 1002U, 1003U},
        }),
        .response_code = asmproto::GeneralResponse::successful,
    }));
    REQUIRE(to_hex(frame) == "060e2b3402050101020701021300000083000019000000020000000300000004000003e9000003ea000003eb00");
}

TEST_CASE(protocol_vector_le_key_load_request_frame_stable) {
    const auto frame = asmproto::Klv::encode(asmproto::Codec::encode_request({
        .command = asmproto::Command::le_key_load,
        .request_id = 3U,
        .payload = asmproto::LeKeyPayloadCodec::encode_load_request({
            .keys = {make_key(4001U)},
        }),
    }));
    REQUIRE(to_hex(frame) == "060e2b340205010102070103200000008300002c00000003000000010000002000000fa100112233445566778899aabbccddeeff00000e1000000000075bcd15");
}

TEST_CASE(protocol_vector_query_spb_response_frame_stable) {
    const auto frame = asmproto::Klv::encode(asmproto::Codec::encode_response({
        .command = asmproto::Command::query_spb,
        .request_id = 4U,
        .payload = asmproto::QuerySpbPayloadCodec::encode_response({
            .protocol_version = 0x01,
            .status = asmproto::QuerySpbStatus::not_playing,
        }),
        .response_code = asmproto::GeneralResponse::successful,
    }));
    REQUIRE(to_hex(frame) == "060e2b340205010102070102170000008300000700000004010000");
}

TEST_CASE(protocol_vector_badrequest_response_frame_stable) {
    const auto request_copy = asmproto::Klv::encode(asmproto::Codec::encode_request({
        .command = asmproto::Command::get_time,
        .request_id = 1U,
        .payload = {},
    }));
    const auto frame = asmproto::Klv::encode(asmproto::Codec::encode_response({
        .command = asmproto::Command::bad_request,
        .request_id = std::nullopt,
        .payload = request_copy,
        .response_code = asmproto::GeneralResponse::invalid,
    }));
    REQUIRE(to_hex(frame) == "060e2b3402050101020701010100000083000019060e2b34020501010207010210000000830000040000000102");
}

#include "asm/codec.hpp"
#include "asm/event_log.hpp"
#include <filesystem>
#include <memory>

#include "asm/le_key.hpp"
#include "asm/service.hpp"
#include "asm/query_spb.hpp"
#include "klv/klv.hpp"

#include "testkit.hpp"

namespace {

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

TEST_CASE(service_handles_get_time_request) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_time,
            .request_id = 77U,
            .payload = {},
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::get_time);
    REQUIRE(result.response.request_id == 77U);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    REQUIRE(result.response.payload.size() == 8U);
}

TEST_CASE(service_rejects_zero_request_id_with_badrequest) {
    asmproto::AsmService service;
    const auto raw_request = asmproto::Klv::encode(asmproto::Codec::encode_request({
        .command = asmproto::Command::get_time,
        .request_id = 0U,
        .payload = {},
    }));
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_time,
            .request_id = 0U,
            .payload = {},
        },
        raw_request,
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::bad_request);
    REQUIRE(result.response.request_id == std::nullopt);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::invalid);
    REQUIRE(result.response.payload == raw_request);
}

TEST_CASE(service_handles_query_spb_request) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::query_spb,
            .request_id = 31U,
            .payload = {},
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::query_spb);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    const auto payload = asmproto::QuerySpbPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.protocol_version == 0x01U);
    REQUIRE(payload.status == asmproto::QuerySpbStatus::not_playing);
}

TEST_CASE(service_handles_get_event_list_request) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_event_list,
            .request_id = 88U,
            .payload = asmproto::GetEventListPayloadCodec::encode_request({
                .time_start = 1776758400U,
                .time_stop = 1776759000U,
            }),
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::get_event_list);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    const auto payload = asmproto::GetEventListPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.event_ids.size() == 3U);
    REQUIRE(payload.event_ids[0] == 1001U);
    REQUIRE(payload.event_ids[2] == 1003U);
}

TEST_CASE(service_rejects_invalid_event_time_window) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_event_list,
            .request_id = 89U,
            .payload = asmproto::GetEventListPayloadCodec::encode_request({
                .time_start = 200U,
                .time_stop = 100U,
            }),
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::get_event_list);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::invalid);
}

TEST_CASE(service_handles_get_event_id_request) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_event_id,
            .request_id = 90U,
            .payload = asmproto::GetEventIdPayloadCodec::encode_request({
                .event_id = 1002U,
            }),
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::get_event_id);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    const auto payload = asmproto::GetEventIdPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.log_record_xml.find("<EventID>1002</EventID>") != std::string::npos);
    REQUIRE(payload.log_record_xml.find("TLS profile uses development CA") != std::string::npos);
}

TEST_CASE(service_rejects_unknown_event_id) {
    asmproto::AsmService service;
    const auto result = service.handle_request(
        {
            .command = asmproto::Command::get_event_id,
            .request_id = 91U,
            .payload = asmproto::GetEventIdPayloadCodec::encode_request({
                .event_id = 9999U,
            }),
        },
        {},
        asmproto::RequestContext {});

    REQUIRE(result.response.command == asmproto::Command::get_event_id);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::invalid);
}

TEST_CASE(service_supports_injected_event_log_provider) {
    auto provider = std::make_shared<asmproto::InMemoryEventLogProvider>(std::vector<asmproto::EventRecord> {
        asmproto::EventRecord {
            .event_id = 2001U,
            .severity = asmproto::EventSeverity::info,
            .timestamp_epoch = 1776762000U,
            .timestamp_text = "2026-04-21T09:00:00Z",
            .summary = "Injected event",
        },
    });
    asmproto::AsmService service(provider);

    const auto list_result = service.handle_request(
        {
            .command = asmproto::Command::get_event_list,
            .request_id = 92U,
            .payload = asmproto::GetEventListPayloadCodec::encode_request({
                .time_start = 1776761999U,
                .time_stop = 1776762000U,
            }),
        },
        {},
        asmproto::RequestContext {});
    const auto list_payload = asmproto::GetEventListPayloadCodec::decode_response(list_result.response.payload);
    REQUIRE(list_payload.event_ids.size() == 1U);
    REQUIRE(list_payload.event_ids[0] == 2001U);

    const auto detail_result = service.handle_request(
        {
            .command = asmproto::Command::get_event_id,
            .request_id = 93U,
            .payload = asmproto::GetEventIdPayloadCodec::encode_request({
                .event_id = 2001U,
            }),
        },
        {},
        asmproto::RequestContext {});
    const auto detail_payload = asmproto::GetEventIdPayloadCodec::decode_response(detail_result.response.payload);
    REQUIRE(detail_payload.log_record_xml.find("Injected event") != std::string::npos);
}

TEST_CASE(jsonl_event_log_provider_loads_events) {
    const auto path = std::filesystem::path(ASM_SOURCE_DIR) / "fixtures/events/default-events.jsonl";
    asmproto::JsonlEventLogProvider provider(path);

    const auto event_ids = provider.list_event_ids(1776758400U, 1776759000U);
    REQUIRE(event_ids.size() == 3U);
    REQUIRE(event_ids[1] == 1002U);

    const auto event_xml = provider.get_event_xml(1003U);
    REQUIRE(event_xml.has_value());
    REQUIRE(event_xml->find("Link encryption material not loaded") != std::string::npos);
}

TEST_CASE(service_handles_le_key_load_query_and_purge) {
    asmproto::AsmService service;

    const auto load_result = service.handle_request(
        {
            .command = asmproto::Command::le_key_load,
            .request_id = 101U,
            .payload = asmproto::LeKeyPayloadCodec::encode_load_request({
                .keys = {make_key(5001U)},
            }),
        },
        {},
        asmproto::RequestContext {});
    REQUIRE(load_result.response.command == asmproto::Command::le_key_load);
    const auto load_payload = asmproto::LeKeyPayloadCodec::decode_load_response(load_result.response.payload);
    REQUIRE(!load_payload.overflow);

    const auto query_result = service.handle_request(
        {
            .command = asmproto::Command::le_key_query_id,
            .request_id = 102U,
            .payload = asmproto::LeKeyPayloadCodec::encode_query_id_request({
                .key_id = 5001U,
            }),
        },
        {},
        asmproto::RequestContext {});
    REQUIRE(query_result.response.command == asmproto::Command::le_key_query_id);
    const auto query_payload = asmproto::LeKeyPayloadCodec::decode_query_id_response(query_result.response.payload);
    REQUIRE(query_payload.key_present);

    const auto purge_result = service.handle_request(
        {
            .command = asmproto::Command::le_key_purge_id,
            .request_id = 103U,
            .payload = asmproto::LeKeyPayloadCodec::encode_purge_id_request({
                .key_id = 5001U,
            }),
        },
        {},
        asmproto::RequestContext {});
    REQUIRE(purge_result.response.command == asmproto::Command::le_key_purge_id);
    const auto purge_payload = asmproto::LeKeyPayloadCodec::decode_purge_id_response(purge_result.response.payload);
    REQUIRE(!purge_payload.no_key_id);
}

TEST_CASE(service_handles_le_key_query_all_and_purge_all) {
    asmproto::AsmService service;

    static_cast<void>(service.handle_request(
        {
            .command = asmproto::Command::le_key_load,
            .request_id = 104U,
            .payload = asmproto::LeKeyPayloadCodec::encode_load_request({
                .keys = {make_key(6001U), make_key(6002U)},
            }),
        },
        {},
        asmproto::RequestContext {}));

    const auto query_all_result = service.handle_request(
        {
            .command = asmproto::Command::le_key_query_all,
            .request_id = 105U,
            .payload = {},
        },
        {},
        asmproto::RequestContext {});
    REQUIRE(query_all_result.response.command == asmproto::Command::le_key_query_all);
    const auto query_all_payload = asmproto::LeKeyPayloadCodec::decode_query_all_response(query_all_result.response.payload);
    REQUIRE(query_all_payload.key_ids.size() == 2U);

    const auto purge_all_result = service.handle_request(
        {
            .command = asmproto::Command::le_key_purge_all,
            .request_id = 106U,
            .payload = {},
        },
        {},
        asmproto::RequestContext {});
    REQUIRE(purge_all_result.response.command == asmproto::Command::le_key_purge_all);
    REQUIRE(purge_all_result.response.response_code == asmproto::GeneralResponse::successful);
}

TEST_CASE(service_reports_le_key_overflow) {
    asmproto::AsmService service;
    std::vector<asmproto::LeKeyRecord> keys;
    for (std::uint32_t i = 0; i < 17U; ++i) {
        keys.push_back(make_key(7000U + i));
    }

    const auto result = service.handle_request(
        {
            .command = asmproto::Command::le_key_load,
            .request_id = 107U,
            .payload = asmproto::LeKeyPayloadCodec::encode_load_request({
                .keys = keys,
            }),
        },
        {},
        asmproto::RequestContext {});
    const auto payload = asmproto::LeKeyPayloadCodec::decode_load_response(result.response.payload);
    REQUIRE(payload.overflow);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::failed);
}

TEST_CASE(jsonl_le_key_store_persists_state) {
    const auto path = std::filesystem::path(ASM_SOURCE_DIR) / "tmp" / "test-le-keys.jsonl";
    std::filesystem::create_directories(path.parent_path());
    std::filesystem::remove(path);

    {
        asmproto::JsonlLeKeyStore store(path);
        REQUIRE(!store.load_keys(std::vector<asmproto::LeKeyRecord> {make_key(8001U)}));
        REQUIRE(store.list_key_ids().size() == 1U);
    }

    {
        asmproto::JsonlLeKeyStore store(path);
        REQUIRE(store.contains_key(8001U));
    }

    std::filesystem::remove(path);
}

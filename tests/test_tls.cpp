#include "asm/codec.hpp"
#include "asm/event_log.hpp"
#include "asm/le_key.hpp"
#include "asm/query_spb.hpp"
#include "asm/service.hpp"
#include "transport/tls_client.hpp"
#include "transport/tls_server.hpp"

#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <thread>

#include "testkit.hpp"

namespace {

const auto kFixtureDir = std::filesystem::path(ASM_SOURCE_DIR) / "fixtures/certs/dev";
const std::uint16_t kBasePort = 18173U;

asmproto::TlsConfig config_for(std::string_view stem) {
    return asmproto::TlsConfig {
        .certificate = kFixtureDir / (std::string(stem) + ".cert.pem"),
        .private_key = kFixtureDir / (std::string(stem) + ".key.pem"),
        .ca_certificate = kFixtureDir / "ca.cert.pem",
    };
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

class ServerHarness {
public:
    explicit ServerHarness(std::uint16_t port)
        : server_(config_for("server"), asmproto::AsmService {}, port) {
        worker_ = std::thread([this] { server_.run(stop_); });
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    ~ServerHarness() {
        stop_.store(true);
        if (worker_.joinable()) {
            worker_.join();
        }
    }

private:
    std::atomic_bool stop_ {false};
    asmproto::TlsServer server_;
    std::thread worker_;
};

asmproto::StandardRequest make_get_time_request() {
    return {
        .command = asmproto::Command::get_time,
        .request_id = 1234U,
        .payload = {},
    };
}

}  // namespace

TEST_CASE(tls_gettime_roundtrip_succeeds) {
    ServerHarness server(kBasePort);
    asmproto::TlsClient client(config_for("client"));

    const auto result = client.send_request("127.0.0.1", kBasePort, make_get_time_request(), std::chrono::seconds(5));
    REQUIRE(!result.peer_thumbprint.empty());
    REQUIRE(result.response.command == asmproto::Command::get_time);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    REQUIRE(result.response.request_id == 1234U);
    REQUIRE(result.response.payload.size() == 8U);
}

TEST_CASE(tls_invalid_request_id_returns_badrequest) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 1U));
    asmproto::TlsClient client(config_for("client"));

    asmproto::StandardRequest request {
        .command = asmproto::Command::get_time,
        .request_id = 0U,
        .payload = {},
    };

    const auto result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 1U), request,
                                            std::chrono::seconds(5));
    REQUIRE(result.response.command == asmproto::Command::bad_request);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::invalid);
    REQUIRE(!result.response.request_id.has_value());
}

TEST_CASE(tls_query_spb_roundtrip_succeeds) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 5U));
    asmproto::TlsClient client(config_for("client"));

    const auto result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 5U),
                                            {
                                                .command = asmproto::Command::query_spb,
                                                .request_id = 555U,
                                                .payload = {},
                                            },
                                            std::chrono::seconds(5));
    REQUIRE(result.response.command == asmproto::Command::query_spb);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    REQUIRE(result.response.request_id == 555U);
    const auto payload = asmproto::QuerySpbPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.protocol_version == 0x01U);
    REQUIRE(payload.status == asmproto::QuerySpbStatus::not_playing);
}

TEST_CASE(tls_get_event_list_roundtrip_succeeds) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 6U));
    asmproto::TlsClient client(config_for("client"));

    const auto result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 6U),
                                            {
                                                .command = asmproto::Command::get_event_list,
                                                .request_id = 777U,
                                                .payload = asmproto::GetEventListPayloadCodec::encode_request({
                                                    .time_start = 1776758400U,
                                                    .time_stop = 1776759000U,
                                                }),
                                            },
                                            std::chrono::seconds(5));
    REQUIRE(result.response.command == asmproto::Command::get_event_list);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    REQUIRE(result.response.request_id == 777U);
    const auto payload = asmproto::GetEventListPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.event_ids.size() == 3U);
    REQUIRE(payload.event_ids[0] == 1001U);
    REQUIRE(payload.event_ids[2] == 1003U);
}

TEST_CASE(tls_get_event_id_roundtrip_succeeds) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 7U));
    asmproto::TlsClient client(config_for("client"));

    const auto result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 7U),
                                            {
                                                .command = asmproto::Command::get_event_id,
                                                .request_id = 778U,
                                                .payload = asmproto::GetEventIdPayloadCodec::encode_request({
                                                    .event_id = 1003U,
                                                }),
                                            },
                                            std::chrono::seconds(5));
    REQUIRE(result.response.command == asmproto::Command::get_event_id);
    REQUIRE(result.response.response_code == asmproto::GeneralResponse::successful);
    REQUIRE(result.response.request_id == 778U);
    const auto payload = asmproto::GetEventIdPayloadCodec::decode_response(result.response.payload);
    REQUIRE(payload.log_record_xml.find("Link encryption material not loaded") != std::string::npos);
}

TEST_CASE(tls_le_key_flow_roundtrip_succeeds) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 8U));
    asmproto::TlsClient client(config_for("client"));

    const auto load_result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 8U),
                                                 {
                                                     .command = asmproto::Command::le_key_load,
                                                     .request_id = 801U,
                                                     .payload = asmproto::LeKeyPayloadCodec::encode_load_request({
                                                         .keys = {make_key(7001U)},
                                                     }),
                                                 },
                                                 std::chrono::seconds(5));
    REQUIRE(load_result.response.command == asmproto::Command::le_key_load);
    const auto load_payload = asmproto::LeKeyPayloadCodec::decode_load_response(load_result.response.payload);
    REQUIRE(!load_payload.overflow);

    const auto query_all_result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 8U),
                                                      {
                                                          .command = asmproto::Command::le_key_query_all,
                                                          .request_id = 802U,
                                                          .payload = {},
                                                      },
                                                      std::chrono::seconds(5));
    REQUIRE(query_all_result.response.command == asmproto::Command::le_key_query_all);
    const auto query_all_payload = asmproto::LeKeyPayloadCodec::decode_query_all_response(query_all_result.response.payload);
    REQUIRE(query_all_payload.key_ids.size() == 1U);
    REQUIRE(query_all_payload.key_ids[0] == 7001U);

    const auto purge_all_result = client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 8U),
                                                      {
                                                          .command = asmproto::Command::le_key_purge_all,
                                                          .request_id = 803U,
                                                          .payload = {},
                                                      },
                                                      std::chrono::seconds(5));
    REQUIRE(purge_all_result.response.command == asmproto::Command::le_key_purge_all);
    REQUIRE(purge_all_result.response.response_code == asmproto::GeneralResponse::successful);
}

TEST_CASE(tls_rejects_missing_client_certificate) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 2U));
    auto client_config = config_for("client");
    client_config.certificate.clear();
    client_config.private_key.clear();

    bool thrown = false;
    try {
        asmproto::TlsClient client(client_config);
        static_cast<void>(client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 2U),
                                              make_get_time_request(), std::chrono::seconds(5)));
    } catch (const std::exception&) {
        thrown = true;
    }
    REQUIRE(thrown);
}

TEST_CASE(tls_rejects_wrong_ca) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 3U));
    auto client_config = config_for("rogue-client");
    client_config.ca_certificate = kFixtureDir / "rogue-ca.cert.pem";

    bool thrown = false;
    try {
        asmproto::TlsClient client(client_config);
        static_cast<void>(client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 3U),
                                              make_get_time_request(), std::chrono::seconds(5)));
    } catch (const std::exception&) {
        thrown = true;
    }
    REQUIRE(thrown);
}

TEST_CASE(tls_rejects_cipher_suite_mismatch) {
    ServerHarness server(static_cast<std::uint16_t>(kBasePort + 4U));
    auto client_config = config_for("client");
    client_config.cipher_list = "ECDHE-RSA-AES256-GCM-SHA384";

    bool thrown = false;
    try {
        asmproto::TlsClient client(client_config);
        static_cast<void>(client.send_request("127.0.0.1", static_cast<std::uint16_t>(kBasePort + 4U),
                                              make_get_time_request(), std::chrono::seconds(5)));
    } catch (const std::exception&) {
        thrown = true;
    }
    REQUIRE(thrown);
}

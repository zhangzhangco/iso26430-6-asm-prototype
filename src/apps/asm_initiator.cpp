#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "asm/codec.hpp"
#include "asm/event_log.hpp"
#include "asm/le_key.hpp"
#include "asm/query_spb.hpp"
#include "transport/tls_client.hpp"

namespace {

asmproto::Command parse_command(std::string_view value) {
    if (value == "get-time") {
        return asmproto::Command::get_time;
    }
    if (value == "query-spb") {
        return asmproto::Command::query_spb;
    }
    if (value == "get-event-list") {
        return asmproto::Command::get_event_list;
    }
    if (value == "get-event-id") {
        return asmproto::Command::get_event_id;
    }
    if (value == "le-key-load") {
        return asmproto::Command::le_key_load;
    }
    if (value == "le-key-query-id") {
        return asmproto::Command::le_key_query_id;
    }
    if (value == "le-key-query-all") {
        return asmproto::Command::le_key_query_all;
    }
    if (value == "le-key-purge-id") {
        return asmproto::Command::le_key_purge_id;
    }
    if (value == "le-key-purge-all") {
        return asmproto::Command::le_key_purge_all;
    }
    throw std::runtime_error("unsupported command");
}

std::array<std::uint8_t, 16> parse_key_hex(std::string_view hex) {
    if (hex.size() != 32U) {
        throw std::runtime_error("LE key hex must be 32 characters");
    }

    std::array<std::uint8_t, 16> out {};
    for (std::size_t i = 0; i < out.size(); ++i) {
        const auto pair = std::string(hex.substr(i * 2U, 2U));
        out[i] = static_cast<std::uint8_t>(std::stoul(pair, nullptr, 16));
    }
    return out;
}

std::string to_hex(std::span<const std::uint8_t> bytes) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (const auto byte : bytes) {
        out << std::setw(2) << static_cast<int>(byte);
    }
    return out.str();
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cerr
            << "usage: asm_initiator <host> <command> [command args...] <cert.pem> <key.pem> <ca.pem>\n";
        return 1;
    }

    const auto command = parse_command(argv[2]);
    int command_argc = 0;
    switch (command) {
        case asmproto::Command::get_time:
        case asmproto::Command::query_spb:
        case asmproto::Command::le_key_query_all:
        case asmproto::Command::le_key_purge_all:
            command_argc = 0;
            break;
        case asmproto::Command::get_event_id:
        case asmproto::Command::le_key_query_id:
        case asmproto::Command::le_key_purge_id:
            command_argc = 1;
            break;
        case asmproto::Command::get_event_list:
            command_argc = 2;
            break;
        case asmproto::Command::le_key_load:
            command_argc = 4;
            break;
        case asmproto::Command::bad_request:
        default:
            std::cerr << "BadRequest is response-only\n";
            return 1;
    }

    const int expected_argc = 3 + command_argc + 3;
    if (argc != expected_argc) {
        std::cerr << "unexpected arguments for command\n";
        return 1;
    }

    const int cert_index = 3 + command_argc;
    asmproto::TlsConfig config {
        .certificate = argv[cert_index],
        .private_key = argv[cert_index + 1],
        .ca_certificate = argv[cert_index + 2],
    };

    asmproto::StandardRequest request;
    request.command = command;
    request.request_id = 1U;

    switch (command) {
        case asmproto::Command::get_event_list:
            request.payload = asmproto::GetEventListPayloadCodec::encode_request({
                .time_start = static_cast<std::uint32_t>(std::stoul(argv[3])),
                .time_stop = static_cast<std::uint32_t>(std::stoul(argv[4])),
            });
            break;
        case asmproto::Command::get_event_id:
            request.payload = asmproto::GetEventIdPayloadCodec::encode_request({
                .event_id = static_cast<std::uint32_t>(std::stoul(argv[3])),
            });
            break;
        case asmproto::Command::le_key_load:
            request.payload = asmproto::LeKeyPayloadCodec::encode_load_request({
                .keys = {
                    asmproto::LeKeyRecord {
                        .key_id = static_cast<std::uint32_t>(std::stoul(argv[3])),
                        .key = parse_key_hex(argv[4]),
                        .expire_time = static_cast<std::uint32_t>(std::stoul(argv[5])),
                        .attribute_data = static_cast<std::uint64_t>(std::stoull(argv[6])),
                    },
                },
            });
            break;
        case asmproto::Command::le_key_query_id:
            request.payload = asmproto::LeKeyPayloadCodec::encode_query_id_request({
                .key_id = static_cast<std::uint32_t>(std::stoul(argv[3])),
            });
            break;
        case asmproto::Command::le_key_purge_id:
            request.payload = asmproto::LeKeyPayloadCodec::encode_purge_id_request({
                .key_id = static_cast<std::uint32_t>(std::stoul(argv[3])),
            });
            break;
        default:
            break;
    }

    asmproto::TlsClient client(config);
    const auto result = client.send_request(argv[1], 1173, request, std::chrono::seconds(5));
    std::cout << "peer=" << result.peer_thumbprint
              << " command=" << asmproto::to_string(result.response.command)
              << " response=" << asmproto::to_string(result.response.response_code);
    if (result.response.request_id.has_value()) {
        std::cout << " request_id=" << *result.response.request_id;
    }

    switch (result.response.command) {
        case asmproto::Command::get_time:
            std::cout << " epoch=" << asmproto::decode_be_u64(result.response.payload, 0U);
            break;
        case asmproto::Command::query_spb: {
            const auto payload = asmproto::QuerySpbPayloadCodec::decode_response(result.response.payload);
            std::cout << " protocol_ver=" << static_cast<int>(payload.protocol_version)
                      << " status=" << asmproto::to_string(payload.status);
            break;
        }
        case asmproto::Command::get_event_list: {
            const auto payload = asmproto::GetEventListPayloadCodec::decode_response(result.response.payload);
            std::cout << " event_count=" << payload.event_ids.size() << " event_ids=";
            for (std::size_t i = 0; i < payload.event_ids.size(); ++i) {
                if (i != 0U) {
                    std::cout << ',';
                }
                std::cout << payload.event_ids[i];
            }
            break;
        }
        case asmproto::Command::get_event_id: {
            const auto payload = asmproto::GetEventIdPayloadCodec::decode_response(result.response.payload);
            std::cout << " xml=" << payload.log_record_xml;
            break;
        }
        case asmproto::Command::le_key_load: {
            const auto payload = asmproto::LeKeyPayloadCodec::decode_load_response(result.response.payload);
            std::cout << " overflow=" << (payload.overflow ? "true" : "false");
            break;
        }
        case asmproto::Command::le_key_query_id: {
            const auto payload = asmproto::LeKeyPayloadCodec::decode_query_id_response(result.response.payload);
            std::cout << " key_present=" << (payload.key_present ? "true" : "false");
            break;
        }
        case asmproto::Command::le_key_query_all: {
            const auto payload = asmproto::LeKeyPayloadCodec::decode_query_all_response(result.response.payload);
            std::cout << " key_count=" << payload.key_ids.size() << " key_ids=";
            for (std::size_t i = 0; i < payload.key_ids.size(); ++i) {
                if (i != 0U) {
                    std::cout << ',';
                }
                std::cout << payload.key_ids[i];
            }
            break;
        }
        case asmproto::Command::le_key_purge_id: {
            const auto payload = asmproto::LeKeyPayloadCodec::decode_purge_id_response(result.response.payload);
            std::cout << " no_key_id=" << (payload.no_key_id ? "true" : "false");
            break;
        }
        case asmproto::Command::bad_request:
            std::cout << " request_copy_hex=" << to_hex(result.response.payload);
            break;
        case asmproto::Command::le_key_purge_all:
        default:
            break;
    }

    std::cout << '\n';
    return 0;
}

#include "asm/service.hpp"

#include <chrono>

namespace asmproto {

namespace {

std::uint64_t current_utc_epoch_seconds() {
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());
}

}  // namespace

AsmService::AsmService()
    : AsmService(std::make_shared<InMemoryEventLogProvider>(), std::make_shared<InMemoryLeKeyStore>()) {}

AsmService::AsmService(std::shared_ptr<const EventLogStore> event_log_store)
    : AsmService(std::move(event_log_store), std::make_shared<InMemoryLeKeyStore>()) {}

AsmService::AsmService(
    std::shared_ptr<const EventLogStore> event_log_store,
    std::shared_ptr<LeKeyStore> le_key_store,
    std::shared_ptr<const QuerySpbStatusProvider> spb_status_provider)
    : event_log_store_(std::move(event_log_store)),
      le_key_store_(std::move(le_key_store)),
      spb_status_provider_(std::move(spb_status_provider)) {}

ServiceResult AsmService::handle_request(
    const StandardRequest& request,
    std::span<const std::uint8_t> raw_request,
    const RequestContext&) const {
    if (request.request_id == 0U) {
        return bad_request(raw_request, "request id must be non-zero");
    }

    switch (request.command) {
        case Command::get_time:
            return handle_get_time(request);
        case Command::get_event_list:
            return handle_get_event_list(request);
        case Command::get_event_id:
            return handle_get_event_id(request);
        case Command::query_spb:
            return handle_query_spb(request);
        case Command::le_key_load:
            return handle_le_key_load(request);
        case Command::le_key_query_id:
            return handle_le_key_query_id(request);
        case Command::le_key_query_all:
            return handle_le_key_query_all(request);
        case Command::le_key_purge_id:
            return handle_le_key_purge_id(request);
        case Command::le_key_purge_all:
            return handle_le_key_purge_all(request);
        case Command::bad_request:
        default:
            return bad_request(raw_request, "unsupported request command");
    }
}

ServiceResult AsmService::handle_decode_failure(std::span<const std::uint8_t> raw_request, std::string_view reason) const {
    return bad_request(raw_request, reason);
}

ServiceResult AsmService::handle_get_time(const StandardRequest& request) const {
    ByteBuffer payload;
    append_be_u64(payload, current_utc_epoch_seconds());
    return {
        .response = {
            .command = Command::get_time,
            .request_id = request.request_id,
            .payload = std::move(payload),
            .response_code = GeneralResponse::successful,
        },
        .note = "GetTime handled",
    };
}

ServiceResult AsmService::handle_get_event_list(const StandardRequest& request) const {
    const auto payload = GetEventListPayloadCodec::decode_request(request.payload);
    if (payload.time_start > payload.time_stop) {
        return {
            .response = {
                .command = Command::get_event_list,
                .request_id = request.request_id,
                .payload = {},
                .response_code = GeneralResponse::invalid,
            },
            .note = "GetEventList rejected because time_start > time_stop",
        };
    }

    return {
        .response = {
            .command = Command::get_event_list,
            .request_id = request.request_id,
            .payload = GetEventListPayloadCodec::encode_response({
                .event_ids = event_log_store_->list_event_ids(payload.time_start, payload.time_stop),
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = "GetEventList handled",
    };
}

ServiceResult AsmService::handle_get_event_id(const StandardRequest& request) const {
    const auto payload = GetEventIdPayloadCodec::decode_request(request.payload);
    const auto xml = event_log_store_->get_event_xml(payload.event_id);
    if (!xml.has_value()) {
        return {
            .response = {
                .command = Command::get_event_id,
                .request_id = request.request_id,
                .payload = {},
                .response_code = GeneralResponse::invalid,
            },
            .note = "GetEventID rejected because event id was not found",
        };
    }

    return {
        .response = {
            .command = Command::get_event_id,
            .request_id = request.request_id,
            .payload = GetEventIdPayloadCodec::encode_response({
                .log_record_xml = *xml,
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = "GetEventID handled",
    };
}

ServiceResult AsmService::handle_query_spb(const StandardRequest& request) const {
    if (!request.payload.empty()) {
        return {
            .response = {
                .command = Command::query_spb,
                .request_id = request.request_id,
                .payload = {},
                .response_code = GeneralResponse::invalid,
            },
            .note = "QuerySPB rejected because payload was not empty",
        };
    }

    return {
        .response = {
            .command = Command::query_spb,
            .request_id = request.request_id,
            .payload = QuerySpbPayloadCodec::encode_response({
                .protocol_version = 0x01,
                .status = spb_status_provider_->current_status(),
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = "QuerySPB handled",
    };
}

ServiceResult AsmService::handle_le_key_load(const StandardRequest& request) const {
    const auto payload = LeKeyPayloadCodec::decode_load_request(request.payload);
    const bool overflow = le_key_store_->load_keys(payload.keys);
    return {
        .response = {
            .command = Command::le_key_load,
            .request_id = request.request_id,
            .payload = LeKeyPayloadCodec::encode_load_response({
                .overflow = overflow,
            }),
            .response_code = overflow ? GeneralResponse::failed : GeneralResponse::successful,
        },
        .note = overflow ? "LEKeyLoad rejected because key buffer would overflow" : "LEKeyLoad handled",
    };
}

ServiceResult AsmService::handle_le_key_query_id(const StandardRequest& request) const {
    const auto payload = LeKeyPayloadCodec::decode_query_id_request(request.payload);
    return {
        .response = {
            .command = Command::le_key_query_id,
            .request_id = request.request_id,
            .payload = LeKeyPayloadCodec::encode_query_id_response({
                .key_present = le_key_store_->contains_key(payload.key_id),
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = "LEKeyQueryID handled",
    };
}

ServiceResult AsmService::handle_le_key_query_all(const StandardRequest& request) const {
    if (!request.payload.empty()) {
        return {
            .response = {
                .command = Command::le_key_query_all,
                .request_id = request.request_id,
                .payload = {},
                .response_code = GeneralResponse::invalid,
            },
            .note = "LEKeyQueryAll rejected because payload was not empty",
        };
    }

    return {
        .response = {
            .command = Command::le_key_query_all,
            .request_id = request.request_id,
            .payload = LeKeyPayloadCodec::encode_query_all_response({
                .key_ids = le_key_store_->list_key_ids(),
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = "LEKeyQueryAll handled",
    };
}

ServiceResult AsmService::handle_le_key_purge_id(const StandardRequest& request) const {
    const auto payload = LeKeyPayloadCodec::decode_purge_id_request(request.payload);
    const bool removed = le_key_store_->purge_key(payload.key_id);
    return {
        .response = {
            .command = Command::le_key_purge_id,
            .request_id = request.request_id,
            .payload = LeKeyPayloadCodec::encode_purge_id_response({
                .no_key_id = !removed,
            }),
            .response_code = GeneralResponse::successful,
        },
        .note = removed ? "LEKeyPurgeID handled" : "LEKeyPurgeID handled with missing key id",
    };
}

ServiceResult AsmService::handle_le_key_purge_all(const StandardRequest& request) const {
    if (!request.payload.empty()) {
        return {
            .response = {
                .command = Command::le_key_purge_all,
                .request_id = request.request_id,
                .payload = {},
                .response_code = GeneralResponse::invalid,
            },
            .note = "LEKeyPurgeAll rejected because payload was not empty",
        };
    }
    le_key_store_->purge_all();
    return {
        .response = {
            .command = Command::le_key_purge_all,
            .request_id = request.request_id,
            .payload = {},
            .response_code = GeneralResponse::successful,
        },
        .note = "LEKeyPurgeAll handled",
    };
}

ServiceResult AsmService::bad_request(std::span<const std::uint8_t> raw_request, std::string_view reason) const {
    ByteBuffer payload(raw_request.begin(), raw_request.end());
    return {
        .response = {
            .command = Command::bad_request,
            .request_id = std::nullopt,
            .payload = std::move(payload),
            .response_code = GeneralResponse::invalid,
        },
        .note = std::string(reason),
    };
}

}  // namespace asmproto

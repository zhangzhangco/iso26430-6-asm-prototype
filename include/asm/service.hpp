#pragma once

#include <memory>

#include "asm/common.hpp"
#include "asm/event_log.hpp"
#include "asm/le_key.hpp"
#include "asm/query_spb.hpp"

namespace asmproto {

class AsmService {
public:
    AsmService();
    explicit AsmService(std::shared_ptr<const EventLogStore> event_log_store);
    AsmService(
        std::shared_ptr<const EventLogStore> event_log_store,
        std::shared_ptr<LeKeyStore> le_key_store,
        std::shared_ptr<const QuerySpbStatusProvider> spb_status_provider = std::make_shared<StaticQuerySpbStatusProvider>());

    ServiceResult handle_request(
        const StandardRequest& request,
        std::span<const std::uint8_t> raw_request,
        const RequestContext& context) const;
    ServiceResult handle_decode_failure(std::span<const std::uint8_t> raw_request, std::string_view reason) const;

private:
    ServiceResult handle_get_time(const StandardRequest& request) const;
    ServiceResult handle_get_event_list(const StandardRequest& request) const;
    ServiceResult handle_get_event_id(const StandardRequest& request) const;
    ServiceResult handle_query_spb(const StandardRequest& request) const;
    ServiceResult handle_le_key_load(const StandardRequest& request) const;
    ServiceResult handle_le_key_query_id(const StandardRequest& request) const;
    ServiceResult handle_le_key_query_all(const StandardRequest& request) const;
    ServiceResult handle_le_key_purge_id(const StandardRequest& request) const;
    ServiceResult handle_le_key_purge_all(const StandardRequest& request) const;

    ServiceResult bad_request(std::span<const std::uint8_t> raw_request, std::string_view reason) const;

    std::shared_ptr<const EventLogStore> event_log_store_;
    std::shared_ptr<LeKeyStore> le_key_store_;
    std::shared_ptr<const QuerySpbStatusProvider> spb_status_provider_;
};

}  // namespace asmproto

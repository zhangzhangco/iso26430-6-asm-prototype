#pragma once

#include <memory>
#include <string>

#include <openssl/ssl.h>

#include "asm/common.hpp"
#include "security/tls_profile.hpp"
#include "transport/tls_context.hpp"

namespace asmproto {

struct ClientExchangeResult {
    StandardResponse response;
    std::string peer_thumbprint;
};

class TlsClient {
public:
    explicit TlsClient(TlsConfig config);
    ClientExchangeResult send_request(
        const std::string& host,
        std::uint16_t port,
        const StandardRequest& request,
        std::chrono::milliseconds timeout);

private:
    TlsConfig config_;
};

}  // namespace asmproto

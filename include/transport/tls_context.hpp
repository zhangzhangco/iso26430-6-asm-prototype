#pragma once

#include <memory>

#include <openssl/ssl.h>

#include "security/tls_profile.hpp"

namespace asmproto {

struct SslCtxDeleter {
    void operator()(SSL_CTX* ctx) const noexcept;
};

using UniqueSslCtx = std::unique_ptr<SSL_CTX, SslCtxDeleter>;

class TlsContextFactory {
public:
    static UniqueSslCtx create_server(const TlsConfig& config);
    static UniqueSslCtx create_client(const TlsConfig& config);
};

}  // namespace asmproto

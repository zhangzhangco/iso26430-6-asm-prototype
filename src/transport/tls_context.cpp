#include "transport/tls_context.hpp"

#include "asm/common.hpp"
#include "security/openssl_init.hpp"
#include "security/tls_profile.hpp"

namespace asmproto {

void SslCtxDeleter::operator()(SSL_CTX* ctx) const noexcept {
    if (ctx != nullptr) {
        SSL_CTX_free(ctx);
    }
}

namespace {

UniqueSslCtx create_ctx(const SSL_METHOD* method, const TlsConfig& config, bool server_mode) {
    ensure_openssl_initialized();

    SSL_CTX* raw = SSL_CTX_new(method);
    if (raw == nullptr) {
        throw AsmError("failed to allocate SSL_CTX");
    }

    UniqueSslCtx ctx(raw);
    TlsProfile::apply(ctx.get(), config, server_mode);
    return ctx;
}

}  // namespace

UniqueSslCtx TlsContextFactory::create_server(const TlsConfig& config) {
    return create_ctx(TLS_server_method(), config, true);
}

UniqueSslCtx TlsContextFactory::create_client(const TlsConfig& config) {
    return create_ctx(TLS_client_method(), config, false);
}

}  // namespace asmproto

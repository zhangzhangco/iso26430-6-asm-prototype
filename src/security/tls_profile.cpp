#include "security/tls_profile.hpp"

#include <openssl/err.h>

#include "asm/common.hpp"
#include "security/certificate_utils.hpp"
#include "security/openssl_init.hpp"

namespace asmproto {

namespace {

void expect(int result, std::string_view message) {
    if (result != 1) {
        throw AsmError(std::string(message));
    }
}

}  // namespace

void TlsProfile::apply(SSL_CTX* ctx, const TlsConfig& config, bool server_mode) {
    ensure_openssl_initialized();

    validate_certificate_profile_from_file(config.certificate);

    expect(SSL_CTX_set_cipher_list(ctx, config.cipher_list.c_str()), "failed to set cipher list");

    if (config.allow_tls10) {
        expect(SSL_CTX_set_min_proto_version(ctx, TLS1_VERSION), "failed to set min TLS version");
        expect(SSL_CTX_set_max_proto_version(ctx, TLS1_VERSION), "failed to set max TLS version");
    }

    SSL_CTX_set_security_level(ctx, 0);
    SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);

    if (!config.certificate.empty() || !config.private_key.empty()) {
        if (config.certificate.empty() || config.private_key.empty()) {
            throw AsmError("certificate and private key must be provided together");
        }

        expect(SSL_CTX_use_certificate_file(ctx, config.certificate.string().c_str(), SSL_FILETYPE_PEM),
               "failed to load certificate");
        expect(SSL_CTX_use_PrivateKey_file(ctx, config.private_key.string().c_str(), SSL_FILETYPE_PEM),
               "failed to load private key");
        expect(SSL_CTX_check_private_key(ctx), "private key mismatch");
    }

    expect(SSL_CTX_load_verify_locations(ctx, config.ca_certificate.string().c_str(), nullptr),
           "failed to load CA certificate");

    int verify_mode = SSL_VERIFY_NONE;
    if (config.verify_peer) {
        verify_mode = SSL_VERIFY_PEER;
        if (server_mode && config.require_peer_certificate) {
            verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        }
    }
    SSL_CTX_set_verify(ctx, verify_mode, nullptr);
}

}  // namespace asmproto

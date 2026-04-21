#include "security/openssl_init.hpp"

#include <mutex>

#include <openssl/err.h>
#include <openssl/ssl.h>

namespace asmproto {

void ensure_openssl_initialized() {
    static std::once_flag once;
    std::call_once(once, [] {
        OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);
        OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);
    });
}

}  // namespace asmproto

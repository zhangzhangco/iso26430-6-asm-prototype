#pragma once

#include <openssl/ssl.h>

#include <filesystem>
#include <string>

namespace asmproto {

struct TlsConfig {
    std::filesystem::path certificate;
    std::filesystem::path private_key;
    std::filesystem::path ca_certificate;
    bool require_peer_certificate {true};
    bool verify_peer {true};
    bool allow_tls10 {true};
    std::string cipher_list {"AES128-SHA"};
};

class TlsProfile {
public:
    static void apply(SSL_CTX* ctx, const TlsConfig& config, bool server_mode);
};

}  // namespace asmproto

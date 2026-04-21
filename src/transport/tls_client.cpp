#include "transport/tls_client.hpp"

#include <memory>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "asm/codec.hpp"
#include "klv/klv.hpp"
#include "security/certificate_utils.hpp"
#include "transport/socket.hpp"

namespace asmproto {

namespace {

struct SslDeleter {
    void operator()(SSL* ssl) const noexcept {
        if (ssl != nullptr) {
            SSL_free(ssl);
        }
    }
};

using UniqueSsl = std::unique_ptr<SSL, SslDeleter>;

void ssl_expect_positive(int rc, SSL* ssl, std::string_view action) {
    if (rc > 0) {
        return;
    }
    const unsigned long error = ERR_get_error();
    const auto ssl_error = SSL_get_error(ssl, rc);
    throw AsmError(std::string(action) + " failed: ssl_error=" + std::to_string(ssl_error)
                   + ", openssl_error=" + std::to_string(error));
}

void write_all(SSL* ssl, std::span<const std::uint8_t> bytes) {
    std::size_t offset = 0U;
    while (offset < bytes.size()) {
        const int rc = SSL_write(ssl, bytes.data() + offset, static_cast<int>(bytes.size() - offset));
        ssl_expect_positive(rc, ssl, "SSL_write");
        offset += static_cast<std::size_t>(rc);
    }
}

ByteBuffer read_one_message(SSL* ssl) {
    ByteBuffer out(4096U);
    const int rc = SSL_read(ssl, out.data(), static_cast<int>(out.size()));
    ssl_expect_positive(rc, ssl, "SSL_read");
    out.resize(static_cast<std::size_t>(rc));
    return out;
}

}  // namespace

TlsClient::TlsClient(TlsConfig config)
    : config_(std::move(config)) {}

ClientExchangeResult TlsClient::send_request(
    const std::string& host,
    std::uint16_t port,
    const StandardRequest& request,
    std::chrono::milliseconds) {
    auto ctx = TlsContextFactory::create_client(config_);
    auto socket = make_tcp_socket();
    socket.connect(host, port);

    UniqueSsl ssl(SSL_new(ctx.get()));
    if (ssl == nullptr) {
        throw AsmError("failed to allocate SSL object");
    }

    SSL_set_fd(ssl.get(), socket.fd());
    ssl_expect_positive(SSL_connect(ssl.get()), ssl.get(), "SSL_connect");

    std::unique_ptr<X509, decltype(&X509_free)> peer_cert(SSL_get1_peer_certificate(ssl.get()), &X509_free);
    validate_certificate_profile(peer_cert.get());
    const auto peer = read_peer_identity(peer_cert.get());

    const auto encoded = Klv::encode(Codec::encode_request(request));
    write_all(ssl.get(), encoded);

    const auto response_bytes = read_one_message(ssl.get());
    const auto packet = Klv::decode(response_bytes);
    return {
        .response = Codec::decode_response(packet),
        .peer_thumbprint = peer.sha256_thumbprint,
    };
}

}  // namespace asmproto

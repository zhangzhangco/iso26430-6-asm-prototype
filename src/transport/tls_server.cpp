#include "transport/tls_server.hpp"

#include <cerrno>
#include <memory>
#include <thread>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "asm/codec.hpp"
#include "klv/klv.hpp"
#include "security/certificate_utils.hpp"
#include "transport/socket.hpp"
#include "transport/tls_context.hpp"

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

TlsServer::TlsServer(TlsConfig config, AsmService service, std::uint16_t port)
    : config_(std::move(config)),
      service_(std::move(service)),
      port_(port) {}

std::uint16_t TlsServer::port() const noexcept {
    return port_;
}

void TlsServer::run(std::atomic_bool& stop_flag, SessionObserver observer) {
    auto ctx = TlsContextFactory::create_server(config_);

    auto server_socket = make_tcp_socket();
    server_socket.set_reuse_addr();
    server_socket.bind_and_listen(port_);

    while (!stop_flag.load()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_socket.fd(), &readfds);

        timeval timeout {};
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;

        const int ready = ::select(server_socket.fd() + 1, &readfds, nullptr, nullptr, &timeout);
        if (ready < 0) {
            if (errno == EINTR) {
                if (stop_flag.load()) {
                    break;
                }
                continue;
            }
            throw AsmError("select failed");
        }
        if (ready == 0) {
            continue;
        }

        auto client_socket = server_socket.accept();
        try {
            serve_client(client_socket.fd(), observer);
        } catch (const std::exception& ex) {
            if (observer) {
                observer(SessionTrace {
                    .note = ex.what(),
                });
            }
        }
        client_socket.close();
    }
}

void TlsServer::serve_client(int client_fd, SessionObserver observer) const {
    auto ctx = TlsContextFactory::create_server(config_);
    UniqueSsl ssl(SSL_new(ctx.get()));
    if (ssl == nullptr) {
        throw AsmError("failed to allocate SSL object");
    }

    SSL_set_fd(ssl.get(), client_fd);
    ssl_expect_positive(SSL_accept(ssl.get()), ssl.get(), "SSL_accept");

    std::unique_ptr<X509, decltype(&X509_free)> peer_cert(SSL_get1_peer_certificate(ssl.get()), &X509_free);
    validate_certificate_profile(peer_cert.get());
    const auto peer = read_peer_identity(peer_cert.get());

    SessionTrace trace;
    trace.peer_thumbprint = peer.sha256_thumbprint;

    const auto request_bytes = read_one_message(ssl.get());
    try {
        const auto klv_packet = Klv::decode(request_bytes);
        const auto request = Codec::decode_request(klv_packet);

        trace.command = request.command;
        trace.request_id = request.request_id;

        const auto result = service_.handle_request(request, request_bytes, RequestContext {
            .peer = peer,
            .received_at = std::chrono::system_clock::now(),
        });
        trace.status = result.response.response_code;
        trace.note = result.note;

        const auto response = Klv::encode(Codec::encode_response(result.response));
        write_all(ssl.get(), response);
    } catch (const std::exception& ex) {
        const auto result = service_.handle_decode_failure(request_bytes, ex.what());
        trace.status = result.response.response_code;
        trace.note = ex.what();
        const auto response = Klv::encode(Codec::encode_response(result.response));
        write_all(ssl.get(), response);
    }

    if (observer) {
        observer(trace);
    }
}

}  // namespace asmproto

#include "transport/socket.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <netdb.h>

#include "asm/common.hpp"

namespace asmproto {

Socket::Socket(int fd)
    : fd_(fd) {}

Socket::~Socket() {
    close();
}

Socket::Socket(Socket&& other) noexcept
    : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

bool Socket::valid() const noexcept {
    return fd_ >= 0;
}

int Socket::fd() const noexcept {
    return fd_;
}

void Socket::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void Socket::set_reuse_addr() const {
    int value = 1;
    if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) != 0) {
        throw AsmError("failed to set SO_REUSEADDR");
    }
}

void Socket::bind_and_listen(std::uint16_t port, int backlog) const {
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (::bind(fd_, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0) {
        throw AsmError(std::string("bind failed: ") + std::strerror(errno));
    }
    if (::listen(fd_, backlog) != 0) {
        throw AsmError(std::string("listen failed: ") + std::strerror(errno));
    }
}

Socket Socket::accept() const {
    const int client_fd = ::accept(fd_, nullptr, nullptr);
    if (client_fd < 0) {
        throw AsmError(std::string("accept failed: ") + std::strerror(errno));
    }
    return Socket(client_fd);
}

void Socket::connect(const std::string& host, std::uint16_t port) const {
    addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* results = nullptr;
    const auto service = std::to_string(port);
    const int rc = ::getaddrinfo(host.c_str(), service.c_str(), &hints, &results);
    if (rc != 0) {
        throw AsmError(std::string("getaddrinfo failed: ") + gai_strerror(rc));
    }

    std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> holder(results, &freeaddrinfo);
    for (addrinfo* current = results; current != nullptr; current = current->ai_next) {
        if (::connect(fd_, current->ai_addr, current->ai_addrlen) == 0) {
            return;
        }
    }

    throw AsmError(std::string("connect failed: ") + std::strerror(errno));
}

Socket make_tcp_socket() {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw AsmError(std::string("socket failed: ") + std::strerror(errno));
    }
    return Socket(fd);
}

}  // namespace asmproto

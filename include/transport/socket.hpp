#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace asmproto {

class Socket {
public:
    explicit Socket(int fd = -1);
    ~Socket();

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int fd() const noexcept;

    void close();
    void set_reuse_addr() const;
    void bind_and_listen(std::uint16_t port, int backlog = 16) const;
    Socket accept() const;
    void connect(const std::string& host, std::uint16_t port) const;

private:
    int fd_;
};

Socket make_tcp_socket();

}  // namespace asmproto

#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <optional>
#include <string>

#include "asm/common.hpp"
#include "asm/service.hpp"
#include "security/tls_profile.hpp"

namespace asmproto {

struct SessionTrace {
    std::string peer_thumbprint;
    Command command {Command::bad_request};
    std::optional<RequestId> request_id;
    GeneralResponse status {GeneralResponse::successful};
    std::string note;
};

using SessionObserver = std::function<void(const SessionTrace&)>;

class TlsServer {
public:
    TlsServer(TlsConfig config, AsmService service, std::uint16_t port = 1173);

    void run(std::atomic_bool& stop_flag, SessionObserver observer = {});
    std::uint16_t port() const noexcept;

private:
    void serve_client(int client_fd, SessionObserver observer) const;

    TlsConfig config_;
    AsmService service_;
    std::uint16_t port_;
};

}  // namespace asmproto

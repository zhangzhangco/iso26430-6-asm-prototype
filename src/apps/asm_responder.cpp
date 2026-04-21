#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>

#include "asm/event_log.hpp"
#include "asm/le_key.hpp"
#include "asm/service.hpp"
#include "transport/tls_server.hpp"

namespace {

std::atomic_bool* g_stop_flag = nullptr;

void handle_signal(int) {
    if (g_stop_flag != nullptr) {
        g_stop_flag->store(true);
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 4 && argc != 5 && argc != 6) {
        std::cerr << "usage: asm_responder <cert.pem> <key.pem> <ca.pem> [events.jsonl] [keys.jsonl]\n";
        return 1;
    }

    asmproto::TlsConfig config {
        .certificate = argv[1],
        .private_key = argv[2],
        .ca_certificate = argv[3],
    };

    std::shared_ptr<const asmproto::EventLogStore> event_log_store;
    if (argc >= 5) {
        event_log_store = std::make_shared<asmproto::JsonlEventLogProvider>(argv[4]);
    } else {
        event_log_store = std::make_shared<asmproto::InMemoryEventLogProvider>();
    }

    std::shared_ptr<asmproto::LeKeyStore> le_key_store;
    if (argc == 6) {
        le_key_store = std::make_shared<asmproto::JsonlLeKeyStore>(argv[5]);
    } else {
        le_key_store = std::make_shared<asmproto::InMemoryLeKeyStore>();
    }

    asmproto::AsmService service(event_log_store, le_key_store);
    asmproto::TlsServer server(config, service, 1173);
    std::atomic_bool stop_flag {false};
    g_stop_flag = &stop_flag;
    std::signal(SIGINT, handle_signal);

    server.run(stop_flag, [](const asmproto::SessionTrace& trace) {
        std::cout << "peer=" << trace.peer_thumbprint
                  << " command=" << asmproto::to_string(trace.command)
                  << " response=" << asmproto::to_string(trace.status);
        if (trace.request_id.has_value()) {
            std::cout << " request_id=" << *trace.request_id;
        }
        std::cout << " note=" << trace.note << '\n';
    });
    return 0;
}

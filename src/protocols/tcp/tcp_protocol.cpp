#include "tcp_protocol.h"
#include <netinet/ip.h>
#include <iostream>
#include <netinet/tcp.h>

tcp_protocol::tcp_protocol(bool server) {
    // prevent linux from sending "destination port unreachable" when this class receives packets.
    // because this class doesn't bind any port in the kernel
//    system("sudo iptables -A OUTPUT -p icmp --icmp-type destination-unreachable -j DROP");

    if (server) {
        // opening a file descriptor
        sd = socket(
                AF_INET, // IPv4
                SOCK_STREAM,
                IPPROTO_TCP // TCP
        );
        if (sd == -1) {
            cerr << "can't open socket" << endl;
            return;
        }

        int enabled = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1) {
            cerr << "can't setsockopt reuseaddr" << endl;
            return;
        }
        if (setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled)) == -1) {
            cerr << "can't setsockopt tcp_nodelay" << endl;
            return;
        }

        // binding a socket
        struct sockaddr_in addr;
        memset((void *) &addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5678);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(
                sd,
                (struct sockaddr *) &addr,
                sizeof(addr)
        ) == -1) {
            cerr << "bind err" << endl;
            return;
        }

        // listening on the socket
        if (listen(sd, 1) == -1) {
            cerr << "listen err" << endl;
            return;
        }

        worker = std::thread([&]() -> void {
            while (true) {
                int client_sd;
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);

                // accepting a new session
                cout << "waiting for a tcp session..." << endl;
                client_sd = accept(
                        sd,
                        (struct sockaddr *) &client_addr,
                        &len
                );
                if (client_sd == -1) {
                    cout << "can't accept the session" << endl;
                    continue;
                }
                cout << "a new tcp session accepted..." << endl;

                ip4_addr addr;
                extract_from_network_order(&addr, (uint8_t *) &client_addr.sin_addr.s_addr);

                auto ptr = std::make_unique<tcp_session>(client_sd,
                                                         tcp_session_data{addr, ntohs(client_addr.sin_port), 5678});
                this->session_generator::generate_event(session_t<tcp_session>(1, std::move(ptr)));

            }
        });
    }
}

tcp_protocol::~tcp_protocol() {
//    cout << "tcp_protocol deconstruct" << endl;
    if (worker.joinable())
        worker.detach();
    close(sd);
}

session_t<tcp_session> tcp_protocol::start_session() {
    int session_sd = socket(AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP);
    if (session_sd == -1) {
        throw "can't open socket";
    }

    int enabled = 1;
    if (setsockopt(session_sd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1) {
        throw "can't setsockopt reuseaddr";
    }
    if (setsockopt(session_sd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled)) == -1) {
        throw "can't setsockopt tcp_nodelay";
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, '\x00', sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(next_dest_port.get_next_choice());
//    my_addr.sin_addr.s_addr = htonl(next_addr.get_next_choice().raw);
    write_in_network_order((uint8_t *) &my_addr.sin_addr.s_addr, &next_addr.get_next_choice());

    // bind to local port
    {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(next_source_port.get_next_choice());
        if (bind(session_sd, (struct sockaddr *) &address, sizeof(address)) == -1) {
            throw "can't bind to local port";
        }
    }

    std::cout << "connecting to the tcp server..." << std::endl;
    if (connect(session_sd,
                (struct sockaddr *) &my_addr,
                sizeof(my_addr)
    ) < 0) {
        throw "can't connect to the server";
    }

    std::cout << "connected to the tcp server" << std::endl;

    return session_t<tcp_session>(1,
                                  std::make_unique<tcp_session>(session_sd,
                                                                tcp_session_data{
                                                                        ip4_addr{next_addr.get_next_choice()},
                                                                        next_source_port.get_next_choice(),
                                                                        next_dest_port.get_next_choice()})
    );

}

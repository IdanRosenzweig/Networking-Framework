#include "tcp_protocol.h"
#include "../../linux/error_codes.h"

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
        if (sd == OPEN_ERROR) {
            std::cerr << "can't open socket" << endl;
            return;
        }

        int enabled = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == SETSOCKOPT_ERROR) {
            std::cerr << "can't setsockopt reuseaddr" << endl;
            return;
        }
        if (setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled)) == SETSOCKOPT_ERROR) {
            std::cerr << "can't setsockopt tcp_nodelay" << endl;
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
        ) == BIND_ERROR) {
            std::cerr << "bind err" << endl;
            return;
        }

        // listening on the socket
        if (listen(sd, 1) == LISTEN_ERROR) {
            std::cerr << "listen err" << endl;
            return;
        }

        worker = std::thread([&]() -> void {
            while (true) {

                // accepting a new session
                std::cout << "waiting for a tcp session..." << endl;

                int client_sd;
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                if ((client_sd = accept(
                        sd,
                        (struct sockaddr *) &client_addr,
                        &len
                )) == ACCEPT_ERROR) {
                    std::cout << "can't accept the session" << endl;
                    continue;
                }
                std::cout << "a new tcp session accepted..." << endl;

                ip4_addr addr;
                extract_from_network_order(&addr, (uint8_t *) &client_addr.sin_addr.s_addr);

                this->session_generator::generate_event(tcp_session_type(tcp_session_data{addr,
                                                                                          ntohs(client_addr.sin_port),
                                                                                          5678},
                                                                         std::make_unique<tcp_session_conn>(client_sd))
                );
            }
        });
    }
}

tcp_protocol::~tcp_protocol() {
    worker.detach();
    close(sd);
}

tcp_session_type tcp_protocol::start_session() {
    int session_sd = socket(AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP);
    if (session_sd == SOCKET_ERROR) {
        std::cerr << "can't open socket" << std::endl;
        throw;
    }

    int enabled = 1;
    if (setsockopt(session_sd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == SETSOCKOPT_ERROR) {
        std::cerr << "can't setsockopt reuseaddr" << std::endl;
        throw;
    }
    if (setsockopt(session_sd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled)) == SETSOCKOPT_ERROR) {
        std::cerr << "can't setsockopt tcp_nodelay" << std::endl;
        throw;
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
        if (bind(session_sd, (struct sockaddr *) &address, sizeof(address)) == BIND_ERROR) {
            std::cerr << "can't bind to local port" << std::endl;
            throw;
        }
    }

    std::cout << "connecting to the tcp server..." << std::endl;
    if (connect(session_sd,
                (struct sockaddr *) &my_addr,
                sizeof(my_addr)
    ) == CONNECT_ERROR) {
        std::cerr << "can't connect to the server" << std::endl;
        throw;
    }

    std::cout << "connected to the tcp server" << std::endl;

    return tcp_session_type(tcp_session_data{
                                    ip4_addr{next_addr.get_next_choice()},
                                    next_source_port.get_next_choice(),
                                    next_dest_port.get_next_choice()},
                            std::make_unique<tcp_session_conn>(session_sd)
    );

}

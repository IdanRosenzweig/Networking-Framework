#include "tcp_protocol.h"
#include <netinet/ip.h>
#include <iostream>
#include <memory>
#include <arpa/inet.h>

tcp_protocol::tcp_protocol() {

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

            // accepting a new client
            cout << "waiting for client..." << endl;
            client_sd = accept(
                    sd,
                    (struct sockaddr *) &client_addr,
                    &len
            );
            if (client_sd == -1) {
                cerr << "can't accept the client" << endl;
                continue;
            }

            ip4_addr addr;
            extract_from_network_order(&addr, (uint8_t*) &client_addr.sin_addr.s_addr);
            as_server_sessions.push_back(std::make_unique<tcp_session>(
                    client_sd, addr, 5678, ntohs(client_addr.sin_port)
            ));

        }
    });
}

tcp_protocol::~tcp_protocol() {
    worker.detach();
    close(sd);
}

void tcp_protocol::start_session() {
    int session_sd = socket(AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP);
    if (session_sd == -1) {
        cerr << "can't open socket" << endl;
        return;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, '\x00', sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(next_dest_port.get_next_choice());
//    my_addr.sin_addr.s_addr = htonl(next_addr.get_next_choice().raw);
    write_in_network_order((uint8_t*) &my_addr.sin_addr.s_addr, &next_addr.get_next_choice());

    // bind to local port
    {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(next_source_port.get_next_choice());
        if (bind(session_sd, (struct sockaddr *) &address, sizeof(address)) == -1) {
            cerr << "can't bind to local port" << endl;
            return;
        }
    }

    std::cout << "connecting to daemon..." << std::endl;
    if (connect(session_sd,
                (struct sockaddr *) &my_addr,
                sizeof(my_addr)
    ) < 0) {
        cerr << "can't connect to daemon" << endl;
        return;
    }

    std::cout << "connected to the daemon" << std::endl;

    as_client_sessions.push_back(std::make_unique<tcp_session>(
            session_sd, ip4_addr{next_addr.get_next_choice()}, next_source_port.get_next_choice(),
            next_dest_port.get_next_choice()));

}

cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)


add_executable(Server
        server_main.cpp

        linux/tcp/tcp_conn_server.cpp
        linux/udp/udp_conn_server.cpp

        linux/ip4/ip4_conn_server.cpp
)

add_executable(Client
        client_main.cpp

        linux/tcp/tcp_conn_client.cpp
        linux/udp/udp_conn_client.cpp
        linux/dns/dns_client.cpp
        linux/icmp/icmp_conn_client.cpp
        linux/ether/arp_conn_client.cpp

        linux/ip4/ip4_conn_client.cpp

)
target_link_libraries(Client PRIVATE resolv)
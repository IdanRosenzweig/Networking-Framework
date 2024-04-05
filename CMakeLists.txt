cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)

include(FindPCAP.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(Server
        server_main.cpp

        linux/tcp/tcp_conn_server.cpp
        linux/udp/udp_conn_server.cpp
        linux/ip4/ip4_conn_server.cpp

        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_traffic.cpp

        linux/ether/ethernet_conn_server.cpp

        linux/ether/mac_addr.cpp
)


target_link_libraries(Server ${PCAP_LIBRARY})

add_executable(Client
        client_main.cpp

        linux/tcp/tcp_conn_client.cpp
        linux/udp/udp_conn_client.cpp
        linux/dns/dns_client.cpp
        linux/icmp/icmp_conn_client.cpp
        linux/ether/ethernet_conn_client.cpp
        linux/ether/arp_conn_client.cpp
        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_traffic.cpp

        linux/ip4/ip4_conn_client.cpp

        linux/ether/mac_addr.cpp

)
target_link_libraries(Client ${PCAP_LIBRARY})
#target_link_libraries(Client PRIVATE resolv)
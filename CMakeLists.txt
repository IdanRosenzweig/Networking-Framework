cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)

include(FindPCAP.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(Server
        server_main.cpp

        protocols/ip4/ip4_conn_server.cpp

        linux/tcp/tcp_conn_server.cpp
        protocols/udp/udp_conn_server.cpp

        protocols/ether/ethernet_conn_server.cpp
        protocols/ether/ethernet_conn_client.cpp
        protocols/ether/mac_addr.cpp

        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_traffic.cpp

        linux/network_layer_gateway/network_layer_gateway_client.cpp
        linux/network_layer_gateway/network_layer_gateway_server.cpp
        protocols/ip4/ip4_addr.cpp

        linux/hardware.cpp
        linux/hardware.h
)


target_link_libraries(Server ${PCAP_LIBRARY})

add_executable(Client
        client_main.cpp

        protocols/ip4/ip4_conn_client.cpp

        linux/tcp/tcp_conn_client.cpp
        protocols/udp/udp_conn_client.cpp

        protocols/dns/dns_client.cpp
        protocols/icmp/icmp_conn_client.cpp

        protocols/ether/ethernet_conn_client.cpp
        protocols/ether/mac_addr.cpp
        protocols/arp/arp_conn_client.cpp

        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_traffic.cpp

        linux/network_layer_gateway/network_layer_gateway_client.cpp
        protocols/ip4/ip4_addr.cpp
        abstract/msg_protocol_stack.h
        abstract/basic_recv_interrupt_handler.h
        protocols/dns/dns_types.h
        abstract/basic_recv_conn.h
        abstract/basic_gateway.h
        linux/hardware.cpp
        linux/hardware.h
        linux/ip_level_proxy/ip_proxy_client.cpp
        linux/ip_level_proxy/ip_proxy_client.h


)
target_link_libraries(Client ${PCAP_LIBRARY})
#target_link_libraries(Client PRIVATE resolv)
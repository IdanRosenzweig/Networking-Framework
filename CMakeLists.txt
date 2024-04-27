cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)

include(FindPCAP.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(Server
        server_main.cpp

        protocols/ether/ethernet_protocol.cpp
        protocols/ether/mac_addr.cpp
        linux/hardware.cpp
        linux/hardware.h

        protocols/arp/arp_conn_client.cpp

        protocols/ip4/ip4_protocol.cpp
        protocols/ip4/ip4_addr.cpp
        protocols/ip4/internet_checksum.cpp

        protocols/udp/udp_protocol.cpp

        protocols/tcp/tcp_protocol.cpp
        protocols/tcp/tcp_session.cpp

        protocols/icmp/icmp_protocol.cpp


        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_sniffer.cpp

        linux/network_layer_gateway/network_layer_gateway.cpp


        temp_connections/udp/udp_client.cpp
        temp_connections/udp/udp_server.cpp

        temp_connections/icmp/icmp_connection_client.cpp
        temp_connections/icmp/icmp_connection_server.cpp

        temp_connections/dns_server/dns_server.cpp
        temp_connections/dns_server/dns.cpp

)


target_link_libraries(Server ${PCAP_LIBRARY})

add_executable(Client
        client_main.cpp

        protocols/ether/ethernet_protocol.cpp
        protocols/ether/mac_addr.cpp
        linux/hardware.cpp
        linux/hardware.h

        protocols/arp/arp_conn_client.cpp

        protocols/ip4/ip4_protocol.cpp
        protocols/ip4/ip4_addr.cpp
        protocols/ip4/internet_checksum.cpp

        protocols/tcp/tcp_protocol.cpp
        protocols/tcp/tcp_session.cpp

        protocols/udp/udp_protocol.cpp

        protocols/icmp/icmp_protocol.cpp


        linux/data_link_layer/data_link_layer_gateway.cpp
        linux/data_link_layer/data_link_sniffer.cpp

        linux/network_layer_gateway/network_layer_gateway.cpp


        temp_connections/udp/udp_client.cpp
        temp_connections/udp/udp_server.cpp

        temp_connections/icmp/icmp_connection_client.cpp
        temp_connections/icmp/icmp_connection_server.cpp

        temp_connections/dns_server/dns_client.cpp
        temp_connections/dns_server/dns.cpp

)
target_link_libraries(Client ${PCAP_LIBRARY})
#target_link_libraries(Client PRIVATE resolv)
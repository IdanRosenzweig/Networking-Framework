cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)

include(FindPCAP.cmake)
include(FindLibSSH.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(Server
        src/abstract/protocol_t.cpp

        src/server_main.cpp

        src/protocols/ether/ethernet_protocol.cpp
        src/protocols/ether/mac_addr.cpp
        src/linux/hardware.cpp
        src/linux/virtual_if.cpp

        src/protocols/arp/arp_protocol.cpp

        src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp

        src/protocols/udp/udp_protocol.cpp

        src/protocols/tcp/tcp_protocol.cpp
        src/protocols/tcp/tcp_session.cpp

        src/protocols/icmp/icmp_protocol.cpp


        src/linux/data_link_layer/data_link_layer_gateway.cpp
        src/linux/data_link_layer/data_link_sniffer.cpp

        src/linux/network_layer_gateway/network_layer_gateway.cpp


        src/temp_connections/udp/udp_client.cpp
        src/temp_connections/udp/udp_server.cpp

        src/temp_connections/icmp/icmp_connection_client.cpp
        src/temp_connections/icmp/icmp_connection_server.cpp

        src/temp_connections/dns_server/dns_server.cpp
        src/temp_connections/dns_server/dns.cpp

        src/analyzer/analyzer.cpp
        src/analyzer/packet_displayer.cpp

        src/proxy/network_layer/ip_proxy_server.cpp

        src/temp_connections/ssh/ssh_server.cpp
        src/vpn/vpn_daemon.cpp
        src/vpn/vpn_daemon.h
        src/abstract/connection/basic_forwarder_daemon.cpp

)


target_link_libraries(Server ${PCAP_LIBRARY})
target_link_libraries(Server ${LIBSSH_LIBRARIES})

add_executable(Client
        src/abstract/protocol_t.cpp

        src/client_main.cpp

        src/protocols/ether/ethernet_protocol.cpp
        src/protocols/ether/mac_addr.cpp
        src/linux/hardware.cpp
        src/linux/virtual_if.cpp

        src/protocols/arp/arp_protocol.cpp

        src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp

        src/protocols/tcp/tcp_protocol.cpp
        src/protocols/tcp/tcp_session.cpp

        src/protocols/udp/udp_protocol.cpp

        src/protocols/icmp/icmp_protocol.cpp


        src/linux/data_link_layer/data_link_layer_gateway.cpp
        src/linux/data_link_layer/data_link_sniffer.cpp

        src/linux/network_layer_gateway/network_layer_gateway.cpp


        src/temp_connections/udp/udp_client.cpp
        src/temp_connections/udp/udp_server.cpp

        src/temp_connections/icmp/icmp_connection_client.cpp
        src/temp_connections/icmp/icmp_connection_server.cpp

        src/temp_connections/dns_server/dns_client.cpp
        src/temp_connections/dns_server/dns.cpp

        src/analyzer/analyzer.cpp
        src/analyzer/packet_displayer.cpp

        src/protocols/socket/socket_msg.h

        src/linux/virtual_if.cpp

        src/temp_connections/ssh/ssh_conn_session.cpp
        src/vpn/vpn_client.cpp
        src/vpn/vpn_client.h

)
target_link_libraries(Client ${PCAP_LIBRARY})
target_link_libraries(Client ${LIBSSH_LIBRARIES})

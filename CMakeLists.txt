cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)

include(FindPCAP.cmake)
include(FindLibSSH.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(PROTOCOLS_SRC
        src/protocols/ether/ethernet_protocol.cpp
        src/protocols/ether/mac_addr.cpp

        src/protocols/arp/arp_protocol.cpp

        src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp

        src/protocols/tcp/tcp_protocol.cpp
        src/protocols/tcp/tcp_session.cpp

        src/protocols/udp/udp_protocol.cpp

        src/protocols/icmp/icmp_protocol.cpp

        src/protocols/msg_boundary/msg_boundary_protocol.cpp

)

set(ABSTRACT_SRC
        src/abstract/protocol_t.cpp
        src/abstract/connection/conn_aggregator.cpp
)

set(LINUX_SRC
        src/linux/data_link_layer/data_link_layer_gateway.cpp
        src/linux/data_link_layer/data_link_sniffer.cpp

        src/linux/network_layer_gateway/network_layer_gateway.cpp

        src/linux/hardware.cpp
        src/linux/virtual_if.cpp
        src/linux/tun_tap.cpp
        src/linux/tun_tap.h
)

add_executable(Server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/server_main.cpp

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

        src/temp_connections/tcp/tcp_server.cpp
        src/main/echo_server/echo_server.cpp
        src/temp_connections/tcp/tcp_boundary_preserving_server.cpp
        src/temp_connections/tcp/tcp_boundary_preserving_server.h

)
target_link_libraries(Server ${PCAP_LIBRARY})
target_link_libraries(Server ${LIBSSH_LIBRARIES})

add_executable(Client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/client_main.cpp

        src/temp_connections/udp/udp_client.cpp
        src/temp_connections/udp/udp_server.cpp

        src/temp_connections/icmp/icmp_connection_client.cpp
        src/temp_connections/icmp/icmp_connection_server.cpp

        src/temp_connections/dns_server/dns_client.cpp
        src/temp_connections/dns_server/dns.cpp

        src/analyzer/analyzer.cpp
        src/analyzer/packet_displayer.cpp


        src/linux/virtual_if.cpp

        src/temp_connections/ssh/ssh_conn_session.cpp

        src/temp_connections/tcp/tcp_client.cpp
        src/temp_connections/tcp/tcp_client.h
        src/main/echo_server/echo_client.cpp
        src/temp_connections/tcp/tcp_boundary_preserving_client.cpp

)
target_link_libraries(Client ${PCAP_LIBRARY})
target_link_libraries(Client ${LIBSSH_LIBRARIES})


add_executable(vpn_daemon
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_daemon_main.cpp
)
target_link_libraries(vpn_daemon ${PCAP_LIBRARY})
target_link_libraries(vpn_daemon ${LIBSSH_LIBRARIES})

add_executable(vpn_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_client_main.cpp

        src/temp_connections/dns_server/dns_client.cpp
        src/temp_connections/dns_server/dns.cpp
)
target_link_libraries(vpn_client ${PCAP_LIBRARY})
target_link_libraries(vpn_client ${LIBSSH_LIBRARIES})


add_executable(echo_server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/temp_connections/tcp/tcp_server.cpp
        src/main/echo_server/echo_server.cpp
)
target_link_libraries(echo_server ${PCAP_LIBRARY})
target_link_libraries(echo_server ${LIBSSH_LIBRARIES})

add_executable(echo_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/temp_connections/tcp/tcp_client.cpp
        src/main/echo_server/echo_client.cpp
)
target_link_libraries(echo_client ${PCAP_LIBRARY})
target_link_libraries(echo_client ${LIBSSH_LIBRARIES})
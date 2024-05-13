cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 20)

include(FindPCAP.cmake)
include(FindLibSSH.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(PROTOCOLS_SRC
        src/protocols/ether/ethernet_protocol.cpp
        src/protocols/ether/mac_addr.cpp

        src/temp_utils/net_arp/net_arp.cpp

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
        src/abstract/utils/datadump.h
)

set(LINUX_SRC
        src/linux/data_link_layer/data_link_layer_gateway.cpp
#        src/linux/data_link_layer/data_link_sniffer.cpp
        src/linux/data_link_layer/interface_sniffer.cpp

        src/linux/network_layer_gateway/network_layer_gateway.cpp

        src/linux/hardware.cpp

        src/linux/virtual_if.cpp
        src/linux/tun_tap.cpp
)

add_executable(Server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/server_main.cpp

        src/temp_connections/udp_client_server/udp_client.cpp
        src/temp_connections/udp_client_server/udp_server.cpp

        src/temp_connections/icmp/icmp_connection_client.cpp
        src/temp_connections/icmp/icmp_connection_server.cpp

        src/temp_utils/dns_server_client/dns_server.cpp
        src/temp_utils/dns_server_client/dns.cpp

        src/temp_utils/analyzer/net_analyzer.cpp
        src/temp_utils/analyzer/packet_displayer.cpp


        src/temp_connections/ssh/ssh_server.cpp

        src/temp_connections/tcp_client_server/tcp_server.cpp
        src/temp_connections/tcp_client_server/tcp_boundary_preserving_server.cpp

        src/temp_utils/onion_network/onion_network_node.cpp
        src/temp_utils/onion_network/onion_network_node.h

)
target_link_libraries(Server ${PCAP_LIBRARY})
target_link_libraries(Server ${LIBSSH_LIBRARIES})

add_executable(Client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/client_main.cpp

        src/temp_connections/udp_client_server/udp_client.cpp
        src/temp_connections/udp_client_server/udp_server.cpp

        src/temp_connections/icmp/icmp_connection_client.cpp
        src/temp_connections/icmp/icmp_connection_server.cpp

        src/temp_utils/dns_server_client/dns_client.cpp
        src/temp_utils/dns_server_client/dns.cpp

        src/temp_utils/analyzer/net_analyzer.cpp
        src/temp_utils/analyzer/packet_displayer.cpp


        src/linux/virtual_if.cpp

        src/temp_connections/ssh/ssh_conn_session.cpp

        src/temp_connections/tcp_client_server/tcp_client.cpp
        src/temp_connections/tcp_client_server/tcp_boundary_preserving_client.cpp

        src/temp_utils/onion_network/client.h

        src/temp_utils/remote_service/remote_service_daemon.cpp
        src/temp_utils/ping/ping_util.cpp
        src/temp_utils/bandwidth/bandwidth.cpp
        src/temp_utils/dns_server_client/database/database_loader.cpp

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

        src/temp_utils/dns_server_client/dns_client.cpp
        src/temp_utils/dns_server_client/dns.cpp
)
target_link_libraries(vpn_client ${PCAP_LIBRARY})
target_link_libraries(vpn_client ${LIBSSH_LIBRARIES})


add_executable(echo_server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/temp_connections/tcp_client_server/tcp_server.cpp
        src/main/echo_server/echo_server.cpp
)
target_link_libraries(echo_server ${PCAP_LIBRARY})
target_link_libraries(echo_server ${LIBSSH_LIBRARIES})

add_executable(echo_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/temp_connections/tcp_client_server/tcp_client.cpp
        src/main/echo_server/echo_client.cpp
)
target_link_libraries(echo_client ${PCAP_LIBRARY})
target_link_libraries(echo_client ${LIBSSH_LIBRARIES})


add_executable(onion_network_node
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/network_node.cpp
        src/temp_utils/proxy/network_layer/ip_proxy_server.cpp
)
target_link_libraries(onion_network_node ${PCAP_LIBRARY})
target_link_libraries(onion_network_node ${LIBSSH_LIBRARIES})

add_executable(onion_network_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/network_client.cpp
        src/temp_utils/dns_server_client/dns_client.cpp
        src/temp_utils/dns_server_client/dns.cpp
)
target_link_libraries(onion_network_client ${PCAP_LIBRARY})
target_link_libraries(onion_network_client ${LIBSSH_LIBRARIES})


add_executable(ping
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/ping/ping.cpp
        src/temp_utils/ping/ping_util.cpp
)
target_link_libraries(ping ${PCAP_LIBRARY})
target_link_libraries(ping ${LIBSSH_LIBRARIES})

add_executable(traceroute
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/ping/traceroute.cpp
        src/temp_utils/ping/traceroute_util.cpp
)
target_link_libraries(traceroute ${PCAP_LIBRARY})
target_link_libraries(traceroute ${LIBSSH_LIBRARIES})


add_executable(bandwidth
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/bandwidth/bandwidth.cpp
        src/temp_utils/bandwidth/bandwidth.cpp
)
target_link_libraries(bandwidth ${PCAP_LIBRARY})
target_link_libraries(bandwidth ${LIBSSH_LIBRARIES})


add_executable(netcat
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/netcat/netcat.cpp
)
target_link_libraries(netcat ${PCAP_LIBRARY})
target_link_libraries(netcat ${LIBSSH_LIBRARIES})


add_executable(dns_server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_server.cpp
        src/temp_utils/dns_server_client/dns_server.cpp
        src/temp_utils/dns_server_client/database/database_loader.cpp
        src/temp_utils/dns_server_client/dns.cpp
)
target_link_libraries(dns_server ${PCAP_LIBRARY})
target_link_libraries(dns_server ${LIBSSH_LIBRARIES})

add_executable(dns_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_client.cpp
        src/temp_utils/dns_server_client/dns_client.cpp
        src/temp_utils/dns_server_client/dns.cpp
)
target_link_libraries(dns_client ${PCAP_LIBRARY})
target_link_libraries(dns_client ${LIBSSH_LIBRARIES})


find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(net_scanner
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_scanner/net_scanner.cpp
        src/temp_utils/net_arp/net_arp.cpp
)
target_link_libraries(net_scanner ${PCAP_LIBRARY})
target_link_libraries(net_scanner ${LIBSSH_LIBRARIES})
target_link_libraries(net_scanner ${Boost_LIBRARIES})

add_executable(net_intercept
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_intercept/net_intercept.cpp
        src/temp_utils/net_arp/net_arp.cpp
)
target_link_libraries(net_intercept ${PCAP_LIBRARY})
target_link_libraries(net_intercept ${LIBSSH_LIBRARIES})
target_link_libraries(net_intercept ${Boost_LIBRARIES})

add_executable(net_analyzer
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_analyzer/net_analyzer.cpp
        src/temp_utils/analyzer/net_analyzer.cpp
)
target_link_libraries(net_analyzer ${PCAP_LIBRARY})
target_link_libraries(net_analyzer ${LIBSSH_LIBRARIES})
target_link_libraries(net_analyzer ${Boost_LIBRARIES})
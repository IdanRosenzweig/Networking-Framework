cmake_minimum_required(VERSION 3.26)
project(Networking)

set(CMAKE_CXX_STANDARD 20)


# link with libpcap
include(FindPCAP.cmake)
link_libraries(${PCAP_LIBRARY})

# link with libboost
find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# allow threads in linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# add root dir to include path
include_directories(${CMAKE_SOURCE_DIR})

# compile flags
add_compile_options(-Werror=return-type)

set(ABSTRACT_SRC
        # src/protocols/protocol_t.cpp
        src/abstract/connection/conn_aggregator.cpp
        src/utils/network_atomic_queue.h
        src/abstract/sniffer/sniffer.h
        src/abstract/sniffer/basic_sniff_handler.h
        src/linux/if/wrappers/interface_sniffer.cpp
        src/linux/if/wrappers/interface_sniffer.h
)

set(PROTOCOLS_SRC
        src/protocols/ether/ethernet2_protocol.cpp
        src/protocols/ether/ethernet_header.cpp
        src/protocols/ether/mac_addr.cpp

        src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp
        src/protocols/ip4/ip4_header.cpp

        src/protocols/tcp/tcp_protocol.cpp
        src/protocols/tcp/tcp_session.cpp
        src/protocols/tcp/tcp_boundary_preserving_session.cpp

        src/protocols/udp/udp_protocol.cpp
        src/protocols/udp/udp_header.cpp

        src/protocols/icmp/icmp_protocol.cpp
        src/protocols/icmp/icmp_header.cpp

        src/protocols/emp/emp_protocol.cpp
)

set(LINUX_SRC
        lib/linux/hardware.h
        lib/linux/hardware.cpp

        lib/linux/tun_tap.h
        lib/linux/tun_tap.cpp

        lib/linux/linux_iface.h
        lib/linux/linux_iface.cpp

        lib/linux/linux_iface_net_access.h

        # src/linux/osi/data_link_layer_gateway.cpp
        # src/linux/osi/network_layer_gateway.cpp
)

set(DIRECT_CONNECTIONS_SRC
        src/direct_connections/udp/udp_client.cpp
        src/direct_connections/udp/udp_server.cpp

        src/direct_connections/tcp/tcp_client.cpp
        src/direct_connections/tcp/tcp_server.cpp
        src/direct_connections/tcp/tcp_boundary_preserving_client.cpp
        src/direct_connections/tcp/tcp_boundary_preserving_server.cpp

        src/direct_connections/icmp/icmp_client.cpp
        src/direct_connections/icmp/icmp_server.cpp

        src/direct_connections/emp/emp_client.cpp
        src/direct_connections/emp/emp_server.cpp

)

add_executable(test
        ${ABSTRACT_SRC}
        ${LINUX_SRC}

        src/main/test/test.cpp

        src/protocols/ether/ethernet2_protocol.cpp
        src/protocols/ether/ethernet_header.cpp
        src/protocols/ether/mac_addr.cpp

        src/protocols/arp/arp_header.cpp
        src/protocols/arp/arp_protocol.cpp

        # src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp
        src/protocols/ip4/ip4_header.cpp

        src/tools/arp_tool/arp_tool.cpp
)

add_executable(vpn_daemon
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_daemon_main.cpp
        src/tools/vpn/vpn_daemon.cpp
)

add_executable(vpn_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_client_main.cpp

        src/tools/vpn/vpn_client.cpp
)


add_executable(remote_shell_util
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/remote_shell/remote_shell_util.cpp
)


add_executable(onion_network_node
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/onion_network_node_main.cpp
        src/tools/proxy/ip_proxy_server.cpp
        src/tools/onion_network/onion_network_node.cpp
)

add_executable(onion_network_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/onion_network_client_main.cpp
        src/tools/proxy/ip_proxy_client.h
        src/tools/onion_network/onion_network_client.cpp

        src/tools/ping/ping_util.cpp
        src/tools/dns/dns_client.cpp
        src/tools/dns/dns.cpp
)


add_executable(ping
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/ping/ping.cpp
        src/tools/ping/ping_util.cpp
)

add_executable(traceroute
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/ping/traceroute.cpp
        src/tools/ping/traceroute_util.cpp
)


add_executable(bandwidth
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/bandwidth/bandwidth.cpp
        src/tools/bandwidth/bandwidth.cpp
)


add_executable(netcat
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/netcat/netcat.cpp
)


add_executable(dns_server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_server.cpp
        src/tools/dns/dns_server.cpp
        src/tools/dns/dns.cpp
)

add_executable(dns_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_client.cpp
        src/tools/dns/dns_client.cpp
        src/tools/dns/dns.cpp
)






# add_executable(net_scanner
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/net_scanner/net_scanner.cpp
#         src/tools/net_arp/net_arp.cpp
# )

add_executable(arp_scanner
        ${ABSTRACT_SRC}
        ${LINUX_SRC}

        src/main/arp_scanner/arp_scanner.cpp

        src/protocols/ether/ethernet2_protocol.cpp
        src/protocols/ether/ethernet_header.cpp
        src/protocols/ether/mac_addr.cpp

        src/protocols/arp/arp_header.cpp
        src/protocols/arp/arp_protocol.cpp

        # src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp
        src/protocols/ip4/ip4_header.cpp

        src/tools/arp_tool/arp_tool.cpp
)

# add_executable(net_intercept
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/net_intercept/net_intercept.cpp
#         src/tools/net_arp/net_arp.cpp
# )

add_executable(arp_spoofer
        ${ABSTRACT_SRC}
        ${LINUX_SRC}

        src/main/arp_spoofer/arp_spoofer.cpp

        src/protocols/ether/ethernet2_protocol.cpp
        src/protocols/ether/ethernet_header.cpp
        src/protocols/ether/mac_addr.cpp

        src/protocols/arp/arp_header.cpp
        src/protocols/arp/arp_protocol.cpp

        # src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp
        src/protocols/ip4/ip4_header.cpp

        src/tools/arp_tool/arp_tool.cpp
)








add_executable(net_analyzer
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/net_analyzer/net_analyzer.cpp
        src/tools/analyzer/net_analyzer.cpp
)

add_executable(mac_changer
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${DIRECT_CONNECTIONS_SRC}
        ${LINUX_SRC}

        src/main/net_intercept/mac_changer.cpp
        src/tools/net_arp/net_arp.cpp
)


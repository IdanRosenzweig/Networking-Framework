cmake_minimum_required(VERSION 3.26)
project(Networking)

set(CMAKE_CXX_STANDARD 20)

# check build os
if (BUILD_OS STREQUAL "linux")
        message("building for linux os")
        add_compile_definitions(-DBUILD_OS_LINUX)
elseif (BUILD_OS STREQUAL "macos")
        message("building for macos")
        add_compile_definitions(-DBUILD_OS_MACOS)
else()
        message(FATAL_MESSAGE "no build os specified")
endif()

# link with libpcap
include(cmake/FindPCAP.cmake)
link_libraries(${PCAP_LIBRARY})

# link with libboost
find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# pthreads
if (BUILD_OS STREQUAL "linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif (BUILD_OS STREQUAL "macos")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if (BUILD_OS STREQUAL "linux")
        add_compile_definitions(-DBUILD_OS_LINUX)
elseif (BUILD_OS STREQUAL "macos")
        add_compile_definitions(-DBUILD_OS_MACOS)
endif()

# add root dir to include path
include_directories(${CMAKE_SOURCE_DIR})

# compile flags
add_compile_options(-Werror=return-type)

set(ABSTRACT_SRC
        src/abstract/connection/conn_aggregator.cpp
        src/utils/network_atomic_queue.h
        src/abstract/sniffer/sniffer.h
        src/abstract/sniffer/basic_sniff_handler.h
        src/abstract/net_access/net_access.cpp
)

set(PROT_ETHER2_SRC
        src/protocols/ether2/ethernet2_protocol.cpp
        src/protocols/ether2/ethernet2_header.cpp
        src/protocols/ether2/mac_addr.cpp
        src/protocols/ether2/ethertype.cpp
)

set(PROT_ARP_SRC
        src/protocols/arp/arp_header.cpp
        src/protocols/arp/arp_protocol.cpp
)

set(PROT_IP4_SRC
        src/protocols/ip4/ip4_protocol.cpp
        src/protocols/ip4/ip4_addr.cpp
        src/protocols/ip4/internet_checksum.cpp
        src/protocols/ip4/ip4_header.cpp
)

set(PROT_UDP_SRC
        src/protocols/udp/udp_protocol.cpp
        src/protocols/udp/udp_header.cpp
)

set(PROT_ICMP_SRC
        src/protocols/icmp/icmp_protocol.cpp
        src/protocols/icmp/icmp_header.cpp
)

set(PROT_DNS_SRC
        src/protocols/dns/dns.h
        src/protocols/dns/dns.cpp
        src/protocols/dns/dns_database.h
        src/protocols/dns/dns_database.cpp
)

set(PROT_EAP_SRC
        src/protocols/eap/eap_addr.cpp
        src/protocols/eap/eap_header.cpp
        src/protocols/eap/eap_protocol.cpp
)

set(PROT_EMP_SRC
        src/protocols/emp/emp_port.cpp
        src/protocols/emp/emp_header.cpp
        src/protocols/emp/emp_protocol.cpp
)

set(PROTS_SRC
        ${PROT_ETHER2_SRC}
        ${PROT_ARP_SRC}
        ${PROT_IP4_SRC}
        ${PROT_UDP_SRC}
        ${PROT_ICMP_SRC}
        ${PROT_DNS_SRC}
        ${PROT_EAP_SRC}
        ${PROT_EMP_SRC}
)

# set(PROTOCOLS_SRC
#         src/protocols/ether2/ethernet2_protocol.cpp
#         src/protocols/ether2/ethernet2_header.cpp
#         src/protocols/ether2/mac_addr.cpp

#         src/protocols/ip4/ip4_protocol.cpp
#         src/protocols/ip4/ip4_addr.cpp
#         src/protocols/ip4/internet_checksum.cpp
#         src/protocols/ip4/ip4_header.cpp

#         src/protocols/tcp/tcp_protocol.cpp
#         src/protocols/tcp/tcp_session.cpp
#         src/protocols/tcp/tcp_boundary_preserving_session.cpp

#         src/protocols/udp/udp_protocol.cpp
#         src/protocols/udp/udp_header.cpp

#         src/protocols/icmp/icmp_protocol.cpp
#         src/protocols/icmp/icmp_header.cpp

#         src/protocols/emp/emp_protocol.cpp
# )

set(LIB_SRC
        lib/hexdump.h
        lib/hexdump.cpp

        lib/var_len_encoding.h
        lib/var_len_encoding.cpp
)

set(LIB_LINUX_SRC
        lib/linux/hardware.h
        lib/linux/hardware.cpp

        lib/linux/tun_tap.h
        lib/linux/tun_tap.cpp

        lib/linux/linux_iface.h
        lib/linux/linux_iface.cpp

        lib/linux/linux_iface_net_access.h

        lib/linux/virtual_net.cpp
)

set(LIB_MACOS_SRC
        lib/macos/hardware.h
        lib/macos/hardware.cpp

        lib/macos/bpf.h
        lib/macos/bpf.cpp

        lib/macos/macos_iface.h
        lib/macos/macos_iface.cpp

        lib/macos/macos_iface_net_access.h

        lib/macos/virtual_net.cpp
)

set(LIB_OS_SRC)
if(BUILD_OS STREQUAL "linux")
        list(APPEND LIB_OS_SRC ${LIB_LINUX_SRC})
elseif(BUILD_OS STREQUAL "macos")
        list(APPEND LIB_OS_SRC ${LIB_MACOS_SRC})
endif()


# set(DIRECT_CONNECTIONS_SRC
#         src/direct_connections/udp/udp_client.cpp
#         src/direct_connections/udp/udp_server.cpp

#         src/direct_connections/tcp/tcp_client.cpp
#         src/direct_connections/tcp/tcp_server.cpp
#         src/direct_connections/tcp/tcp_boundary_preserving_client.cpp
#         src/direct_connections/tcp/tcp_boundary_preserving_server.cpp

#         src/direct_connections/icmp/icmp_client.cpp
#         src/direct_connections/icmp/icmp_server.cpp

#         src/direct_connections/emp/emp_client.cpp
#         src/direct_connections/emp/emp_server.cpp

# )


# add_executable(vpn_daemon
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/vpn/vpn_daemon_main.cpp
#         src/tools/vpn/vpn_daemon.cpp
# )

# add_executable(vpn_client
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/vpn/vpn_client_main.cpp

#         src/tools/vpn/vpn_client.cpp
# )


# add_executable(remote_shell_util
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/remote_shell/remote_shell_util.cpp
# )


# add_executable(onion_network_node
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/onion_network/onion_network_node_main.cpp
#         src/tools/proxy/ip_proxy_server.cpp
#         src/tools/onion_network/onion_network_node.cpp
# )

# add_executable(onion_network_client
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/onion_network/onion_network_client_main.cpp
#         src/tools/proxy/ip_proxy_client.h
#         src/tools/onion_network/onion_network_client.cpp

#         src/tools/ping/ping_util.cpp
#         src/tools/dns/dns_client.cpp
#         src/tools/dns/dns.cpp
# )


add_executable(ping
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/ping/ping_tool.cpp

        src/main/ping/ping.cpp
)

# add_executable(traceroute
#         ${ABSTRACT_SRC}
#         ${LIB_SRC}
#         ${LIB_OS_SRC}
#         ${PROT_ETHER2_SRC}
#         ${PROT_ARP_SRC}
#         ${PROT_IP4_SRC}
#         ${PROT_ICMP_SRC}

#         src/tools/traceroute/traceroute_tool.cpp

#         src/main/traceroute/traceroute.cpp
# )


# add_executable(bandwidth
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/bandwidth/bandwidth.cpp
#         src/tools/bandwidth/bandwidth.cpp
# )


# add_executable(netcat
#         ${ABSTRACT_SRC}
#         ${PROTOCOLS_SRC}
#         ${DIRECT_CONNECTIONS_SRC}
#         ${LINUX_SRC}

#         src/main/netcat/netcat.cpp
# )


add_executable(dns_server
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/dns/dns_tool.cpp

        src/main/dns/dns_server.cpp
)

add_executable(dns_client
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/dns/dns_tool.cpp

        src/main/dns/dns_client.cpp
)







add_executable(arp_scanner
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/arp_tool/arp_tool.cpp

        src/main/arp_scanner/arp_scanner.cpp
)

add_executable(arp_spoofer
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/arp_tool/arp_tool.cpp

        src/main/arp_spoofer/arp_spoofer.cpp
)

add_executable(netcat_local_ether2
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/main/netcat/netcat_local_ether2.cpp
)

add_executable(netcat_local_ip4
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/main/netcat/netcat_local_ip4.cpp
)

add_executable(net_analyzer
        ${ABSTRACT_SRC}
        ${LIB_SRC}
        ${LIB_OS_SRC}
        ${PROTS_SRC}

        src/tools/net_analyzer/net_analyzer.cpp
        src/tools/net_analyzer/parsed_packet.cpp

        src/main/net_analyzer/net_analyzer.cpp
)


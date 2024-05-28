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


set(ABSTRACT_SRC
        src/abstract/protocol_t.cpp
        src/abstract/connection/conn_aggregator.cpp
        src/abstract/utils/network_msg_queue.h
        src/abstract/sniffer/sniffer.h
        src/abstract/sniffer/basic_sniff_handler.h
        src/linux/if/wrappers/interface_sniffer.cpp
        src/linux/if/wrappers/interface_sniffer.h
)

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

        src/protocols/emp/emp.cpp
        src/protocols/tcp/tcp_boundary_preserving_session.cpp
)

set(LINUX_SRC
        src/linux/if/wrappers/interface_gateway.cpp
        src/linux/if/iface_access_point.cpp

        src/linux/if/hardware.cpp

        src/linux/if/virtual/virtual_if.cpp
        src/linux/if/virtual/tun_tap.cpp

        src/linux/osi/data_link_layer_gateway.cpp
        src/linux/osi/network_layer_gateway.cpp
)


add_executable(vpn_daemon
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_daemon_main.cpp
)

add_executable(vpn_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/vpn/vpn_client_main.cpp

        src/temp_prot_stacks/tcp_client_server/tcp_client.cpp
#        src/temp_utils/dns/dns_client.cpp
#        src/temp_utils/dns/dns.cpp
)


add_executable(remote_shell_util
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/remote_shell_util/remote_shell_util.cpp
        src/temp_prot_stacks/tcp_client_server/tcp_server.cpp
        src/temp_prot_stacks/tcp_client_server/tcp_client.cpp
)


add_executable(onion_network_node
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/network_node.cpp
        src/temp_utils/proxy/ip_proxy_server.cpp
        src/temp_prot_stacks/udp_client_server/udp_server.cpp
)

add_executable(onion_network_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/onion_network/network_client.cpp
        src/temp_utils/dns/dns_client.cpp
        src/temp_utils/dns/dns.cpp
        src/temp_prot_stacks/udp_client_server/udp_client.cpp
        src/temp_utils/ping/ping_util.cpp
)


add_executable(ping
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/ping/ping.cpp
        src/temp_utils/ping/ping_util.cpp
)

add_executable(traceroute
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/ping/traceroute.cpp
        src/temp_utils/ping/traceroute_util.cpp
)


add_executable(bandwidth
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/bandwidth/bandwidth.cpp
        src/temp_utils/bandwidth/bandwidth.cpp
)


add_executable(netcat
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/netcat/netcat.cpp
        src/temp_prot_stacks/tcp_client_server/tcp_client.cpp
        src/temp_prot_stacks/tcp_client_server/tcp_server.cpp
)


add_executable(dns_server
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_server.cpp
        src/temp_utils/dns/dns_server.cpp
        src/temp_utils/dns/database_loader.cpp
        src/temp_utils/dns/dns.cpp
        src/temp_prot_stacks/udp_client_server/udp_server.cpp
        src/temp_prot_stacks/emp/emp_server.cpp
)

add_executable(dns_client
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/dns/dns_client.cpp
        src/temp_utils/dns/dns_client.cpp
        src/temp_utils/dns/dns.cpp
        src/temp_prot_stacks/udp_client_server/udp_client.cpp
        src/temp_prot_stacks/emp/emp_client.cpp
)


add_executable(net_scanner
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_scanner/net_scanner.cpp
        src/temp_utils/net_arp/net_arp.cpp
)

add_executable(net_intercept
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_intercept/net_intercept.cpp
        src/temp_utils/net_arp/net_arp.cpp
)

add_executable(net_analyzer
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_analyzer/net_analyzer.cpp
        src/temp_utils/analyzer/net_analyzer.cpp
)

add_executable(mac_changer
        ${ABSTRACT_SRC}
        ${PROTOCOLS_SRC}
        ${LINUX_SRC}

        src/main/net_intercept/mac_changer.cpp
        src/temp_utils/net_arp/net_arp.cpp
)


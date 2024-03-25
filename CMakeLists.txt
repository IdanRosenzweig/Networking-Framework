cmake_minimum_required(VERSION 3.26)
project(ServerClient)

set(CMAKE_CXX_STANDARD 17)


add_executable(Server
        server_main.cpp

        linux/tcp/linux_server_tcp.cpp
        linux/udp/linux_server_udp.cpp

        linux/ip4/linux_server_ip4.cpp
)

add_executable(Client
        client_main.cpp

        linux/tcp/linux_client_tcp.cpp
        linux/udp/linux_client_udp.cpp
        linux/dns/linux_client_dns.cpp

        linux/ip4/linux_client_ip4.cpp

)
target_link_libraries(Client PRIVATE resolv)
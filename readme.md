# Networking Framework and Tools Suite

## overview
I developed a comprehensive networking framework designed primarily for high-performance processing of raw packets.
The framework implements most modern protocols (specifically the TCP/IP protocol suite) and contains further core networking software and logic, all implemented from scratch.

In addition, I implemented a wide range of modern networking tools which are all built entirely on top of the networking framework:
- Network sniffer & analyzer (like Wireshark/tcpdump)
- Remote access VPN (like openvpn)
- DNS server & client
- Onion network proxies & client (like TOR)
- Secure remote shell control (like SSH shell)
- Firewall with custom filter (like iptables)
- ARP spoofer & scanner (like Ettercap)
- and many more

I also designed and implemented my own protocol called "endpoint multiplexing protocol" (EMP) which provides unbounded, efficient, port-like multiplexing functionality.
It is an alternative to the limited TCP/UDP port range (only ~65,000 values), and uses unbounded arrays of unsigned bytes.

## structure

| what                                                                                                    | docs file                                        |
|---------------------------------------------------------------------------------------------------------|--------------------------------------------------|
| abstract core networking logic used in the project                                                      | [networking_logic](docs/networking_logic.md)     |
| the various protocols implemented in the project                                                        | [protocols](docs/protocols.md)                   |
| my own protocol implemented in the project                                                              | [my own protocol](docs/my_own_protocol.md)       |  
| extending the raw protocols implementations, the proejct includes common aggregation of protocol stacks | [protocol stacks](docs/protocol_stacks.md)       |
| practical, simple, direct communication protocols                                                       | [direct connections](docs/direct_connections.md) |
| many networking tools implemented in the project and their main files                                   | [tools and main files](docs/tools_and_main.md)   |
| linux related features and functions used in the project                                                | [linux](docs/linux_related.md)                   |

## build

the project uses the cmake build system, with the main build file CMakeFile.txt located at the root directory.

aggregation of the abstract files is stored as ABSTRACT_STR.

aggregation of the raw protocols files is stored as PROTOCOLS_SRC.

aggregation of the direct connections files is stored as DIRECT_CONNECTIONS_SRC.

aggregation of the linux related files is stores as LINUX_SRC.

many targets are found within the file, each building some specific tool.

## dependencies

* this project uses the linux pcap library (libpcap) merely to listen to raw packets coming in/out of linux interfaces
  in a synchronized manner.

* this project uses the program_options part of the c++ boost library (libboost) to parse command line arguments.
  if you don't want to depend on boost, change the main files and replace the part that uses boost with your own
  argument parser.

## future TODOs
* actually implemented TCP (current implementation just uses the linux api socket(), bind(), listen(), connect() and wrapps it with `session_generator`)
* implement DHCP protocol and server
* implement ipv4 routing
* implement port scanner like nmap
* implement HTTP/S?
* implement encryption and add it to the vpn tool
* extend the ip_proxy_server to handle multiple clients (currently handles one client at a time)
* implement more records types in dns client & server
* implement a kernel network hook in linux so that a `firewall` can actually block system-wide packets and not only local `connection` classes
* implement SSH?
* implement SOCKS api for the onion network (current implementation merely opens a virtual interface which ip traffic can be sent to and received from)

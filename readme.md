# overview

this project includes core networking software implemented from zero:

* low-level packet analysis and processing framework (similar to the netfilter framework on linux)
* raw implementation of common networking protocols
* aggregation of protocols into widely used protocol stacks and packets

in addition, this project contains many networking tools and classes that are built entirely on top of these networking capabilities:

* servers
* clients
* device scanners
* traffic interceptors
* sniffers
* analyzers
* vpn
* onion network

this project also contains implementation for my own protocol, called "endpoint multiplexing protocol" (EMP) which is an unbounded, port-like multiplexing protocol. this protocol provides an alternative to the limited TCP and UDP ports (only ~65000 possible ports) via unbounded arrays of unsigned bytes.

# structure

| what                                                                                                    | docs file                                      |
|---------------------------------------------------------------------------------------------------------|------------------------------------------------|
| abstract core networking logic used in the project                                                      | [networking_logic](docs/networking_logic.md)   |
| the various protocols implemented in the project                                                        | [protocols](docs/protocols.md)                 |
| my own protocol implemented in the project                                                              | [my own protocol](docs/my_own_protocol.md)     |  
| extending the raw protocols implementations, the proejct includes common aggregation of protocol stacks | [protocol_stacks](docs/protocol_stacks.md)     |
| practical, simple, direct communication connections                                                     | [protocol_stacks](docs/direct_connections.md)  |
| many networking tools implemented in the project and their main files                                   | [tools and main files](docs/tools_and_main.md) |
| linux related features and functions used in the project                                                | [linux](docs/linux_related.md)                 |

# build

the project uses the cmake build system, with the main build file CMakeFile.txt located at the root directory.

aggregation of the abstract files is stored as ABSTRACT_STR.

aggregation of the raw protocols files is stored as PROTOCOLS_SRC.

aggregation of the direct connections files is stored as DIRECT_CONNECTIONS_SRC.

aggregation of the linux related files is stores as LINUX_SRC.

many targets are found within the file, each building some specific tool.

# dependencies

* this project uses the linux pcap library (libpcap) merely to listen to raw packets coming in/out of linux interfaces
  in a synchronized manner.

* this project uses the program_options part of the c++ boost library (libboost) to parse command line arguments.
  if you don't want to depend on boost, change the main files and replace the part that uses boost with your own
  argument parser.

# future TODOs
* actually implemented TCP (current implementation just uses the linux api socket(), bind(), listen(), connect() and wrapps it with `session_generator`)
* implement DHCP protocol and server
* implemented ipv4 routing
* implement port scanner like nmap
* implement HTTP/S?
* implement encryption and add it to the vpn tool
* extend the ip_proxy_server to handle multiple clients (currently handles one client at a time)
* implement more records types in dns client & server
* implement a kernel network hook in linux so that a `firewall` can actually block system-wide packets and not only local `connection` classes
* implement SSH?
* implement SOCKS api for the onion network (current implementation merely opens a virtual interface which ip traffic can be sent to and received from)

# overview

this project includes core networking software implemented from zero:

* low-level packet analysis and processing framework (similar to the netfilter framework on linux)
* raw implementation of common networking protocols
* aggregation of protocols into widely used protocol stacks

in addition, this project contains many networking tools built entirely on top of these networking capabilities:

* servers
* clients
* scanners
* interceptors
* sniffers
* analyzers
* and more...

# structure

| what                                                                                                    | docs file                                    |
|---------------------------------------------------------------------------------------------------------|----------------------------------------------|
| abstract core networking logic used in the project                                                      | [networking_logic](docs/networking_logic.md) |
| the various protocols implemented in the project                                                        | [protocols](docs/protocols.md)               |
| my own protocol implemented in the project                                                              | [my own protocol](docs/my_own_protocol.md)   |  
| extending the raw protocols implementations, the proejct includes common aggregation of protocol stacks | [protocol_stacks](docs/protocol_stacks.md)   |
| practical, simple, direct communication protocols                                                       | [protocol_stacks](docs/protocol_stacks.md)   |
| many networking tools implemented in the project and their main files                                   | [tools and main files](docs/tools_and_main.md)        |
| linux related features and functions used in the project                                                | [linux](docs/linux_related.md)               |

# build

the project uses the cmake build system, with the main build file CMakeFile.txt located at the root directory.

aggregation of the abstract files is stored as ABSTRACT_STR.

aggregation of the raw protocols files is stored as PROTOCOLS_SRC.

aggregation of the linux related files is stores as LINUX_SRC.

many targets are found within the file, each building some specific tool.

# dependencies

* this project uses the linux pcap library (libpcap) merely to listen to raw packets coming in/out of linux interfaces
  in a synchronized manner.

* this project uses the program_options part of the c++ boost library (libboost) to parse command line arguments.
  if you don't want to depend on boost, change the main files and replace the part that uses boost with your own
  argument parser.

# future TODOs

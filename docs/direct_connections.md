# direct connections
using the raw protocols implementation and the protocol stacks, and leaving aside any fancy tools or features, this project includes practical classes for establishing simple communication connections:

under `src/direct_connections/udp`:
* `udp_client.cpp`: udp client that uses udp over ip to send datagrams to a server on a specified port
* `udp_server.cpp`: udp server that uses udp over ip to listen to datagrams coming from clients into a specified port

under `src/direct_connections/icmp`, a little uncommon but still useful (e.g. tunneling for masquerade):
* `icmp_client.cpp`: icmp client that uses icmp over ip to send packets to a server via unregistered icmp type and code (specified in `common.h`)
* `icmp_server.cpp`: icmp server that uses icmp over ip to listen to packets coming from clients via the unregistered icmp type

under `src/direct_connections/tcp`:
* `tcp_client.cpp`: tcp client that uses tcp over ip to establish a session to a server on a specified port
* `tcp_server.cpp`: tcp server that uses tcp over ip to accept client's sessions
* and boundary preserving variants of these: `tcp_boundary_preserving_client.cpp` and `tcp_boundary_preserving_server.cpp`

under `src/direct_connections/emp`, using my own protocol:
* `emp_client.cpp`: emp client that uses emp over ip to send datagrams to a server on a specified endpoint
* `emp_server.cpp`: emp server that uses emp over ip to listen to data coming from clients into a specified endpoint

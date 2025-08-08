# send and receive data
under `src/abstract/sending`:
* `send_medium`: pure virtual class, representing networking logic for sending data
* `send_forwarder`: implementation of `send_medium` for forwarding data that is sent through multiple `send_medium` classes
* `msg/send_msg_t`: a basic unit for transmitting **unsigned bytes (uint8_t)**

under `src/abstract/receiving`:
* `recv_listener`: pure virtual class, representing networking logic for an actor which can handle received data
* `recv_forwarder`: implementation of `recv_listener` for forwarding data that is received through multiple `recv_listener` classes
* `msg/recv_msg_t`: a basic unit for receiving **unsigned bytes (uint8_t)** representing a packet with layers of protocol encapsulation

# connections
under `src/connection/connection`:
* `connection`: a simple class which inherits `send_medium` and `recv_forwarder`, representing a networking login for a communication connection.

# sessions
under `src/connection/session`:
* `session_conn`: just a typedef for `connection`, this represents a networking login for the networking connection part of a communication session
* `session_t`: a class representing a full communication session. this includes both the `session_conn` part and arbitrary data stored regarding the session (priority, initiation time, id...)
* `session_generator`: a class which generates sessions
* `session_handler`: pure virtual class, which represents a handler for a newly generated session. this is essentially a wrapper handler around a session.
* `session_manger`: a class which takes the sessions generated from a `session_generator`, stores them and calls `session_handler` around them.

# gateways
under `src/abstract/gateway`:
* `gateway`: just a typedef for `connection`, this represents a networking login for a gateway you can send data to and listen to data from

# firewalls
under `src/abstract/firewall`:
* `basic_firewall_filter`: pure virtual class, representing a firewall rule which can take data and produce `firewall_policy` decision about it
* `firewall`: a class representing networking logic of a firewall on top of a base `connection`. you can add filters to "outgoing" and "incoming" data. this class takes the base connection, overrides its send and recv functions with its filters, and only allows data which passed all the filters to continue. the firewall class itself inherits `connection`, and represents the base connection after applying the filters.

# sniffers
under `src/abstract/sniffer`:
* `basic_sniff_handler`: pure virtual class, represents networking logic for an actor which handles sniffed data. this class just inherits the pure virtual `recv_listener` without implementing it\
* `sniffer'`: represents aggregation of multiple `basic_sniff_handler` both for sniffing "outgoing" data and "incoming" data.

# utils
under `src/utils` there are some basic data structures and utility functions:
* `udata`: just unsigned bytes and encoding/decoding
* `trie`: generic implementation of a trie data structure
* `circular_queue`: generic implementation of a circular queue data structure with maximum size
* `network_atomic_queue`: just a wrapper class around `circular_queue` which adds atomicity and concurrency
* `multiplexer`: data structure for forwarding streams of data to various listeners. essentially, this class takes variadic amount of templates representing array of types. for example a multiplexer can be `multiplexer<ip4_addr, uint_8>` representing array of two types: a destination address and subprotocl of ipv4 packets. then, listeners can register to be notified about future streams of the array types with specific attributes. regarding the example, say listen for specific subprotocol or specific dest address of the received ipv4 packet (or both).
* `next_choice`: just a wrapper class for single variable, with getter and setter
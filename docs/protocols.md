# protocols
this project includes raw implementations for various networking protocols, including:
* ethernet under `protocols/ether`
* arp under `protocols/arp`
* ipv4 under `protocols/ipv4`
* icmp under `protocols/icmp`
* udp under `protocols/udp`
* tcp under `protocols/tcp`

in addition, a general protocol for preserving message boundaries is implemented under `protocols/msg_boundary`.
(takes a protocol which doesn't necessarily preserve message boundary, like tcp, and wraps it with a message preserving mechanism)

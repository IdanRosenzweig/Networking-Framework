# tools
there are many networking tools in this project which are built project entirely on top pf the networking functionalities implemented in the project.

## ping & traceroute
simple tools for pinging a device connected to ip network and for finding a potential path to that device.

under `src/tools/ping`:
* `ping_util`: the class for pinging devices
* `traceroute_util`: the class for finding paths to devices

## dns
under `src/tools/dns`:
* `dns_client`: simple dns client which can request dns records from a dns server.
* `dns_server`: dns server which will answer dns queries it receives. you can specify a database with various records to be loaded in to the server.

the `dns.cpp` class contains base algorithms and data structures related to dns, which the client and the server use.

## monitors & analyzers
under `src/tools/bandwidth`, there is a file `bandwidth` which is a tools that counts the amount of "outgoing" and "incoming" packets. it essential takes a `sniffer` class and attaches to it two counters, one for outgoing packets and one for incoming packets. you can use this class to monitor the size of the network traffic in constant intervals

under `src/tools/analyzer`, there is a file `net_analyzer` which is a networking tool for displaying the network traffic. it essential takes a sniffer and attaches a handler to both outgoing and incoming packets, which would process the packets and display their protocol stack and also show a hexdump of the raw bytes.

## scanners & interceptors
under `src/tools/net_arp`, there is a class `net_arp` which is a full arp scanner & spoofer. it can be used to:
* search for specific hosts in a local network
* scan entire subnets
* perform MITM arp spoofing attack, both for sniffing traffic and both for completely blocking traffic. attack can be done on specific hosts or entire broadcast
* announce new MAC address for a specific IP address
* and more...

## vpn
under `src/tools/vpn` there is a simple implementation for a vpn which doesn't use encryption but merely boundary-preserving tcp connection:
* `vpn_daemon`: a vpn daemon to deploy on a local network. it listens to a vpn client, and upon connection establishment, sends all local traffic to the vpn client and all the client's traffic onto the local network.
* `vpn_client`: a vpn client which connects to a running daemon. upon connection establishment, sends its related traffic to the vpn daemon, and receives all the remove network's traffic.

## onion network & proxies
under `src/tools/proxy`:
* `ip_proxy_server`: a general proxy server for ipv4. currently only supports a single client. it receives ip packets through the client's `connection` provided to it, stores internally the source address and forwards the packet in the guise of the source address of the server. when received back, the proxy searches for the correct source address and forwards the packet as with the corresponding address.
* `ip_proxy_client`: a general utility for a client to connect to a proxy server. takes any `connection`, and establishes the required connection to the server. (currently just does nothing, the server would receive any packet without premade authentication or handshake).

similar to a TOR network, this is an implementation for a simple onion network for ipv4 under `src/tools/onion_netowrk`:
* `onion_network_node`: a server daemon to deploy. the machine running this server must be somehow connected to the ip network. this class is essentially a wrapper around `ip_proxy_server` which uses udp to listen to clients.
* `onion_netowrk_client`: a client to connect to a path of nodes in the network. this class is a wrapper around `ip_proxy_client` which uses udp to connect to servers.

# tools
there are many networking tools in this project which are built project entirely on top pf the networking functionalities implemented in the project.

## ping & traceroute
under `src/tools/ping` there are simple tools for pinging a device connected to ip network and for finding a potential path to that device.

**ping_util**:

- class for pinging devices.
- the main file is in `src/main/ping/ping.cpp`

***traceroute_util***:

- class for finding paths to devices
- the main file is in `src/main/ping/traceroute.cpp`

## dns
under `src/tools/dns`

**common**:

the `dns.cpp` class contains base algorithms and data structures related to dns, which the client and the server use.

**dns_client**:
- simple dns client which can request dns records from a dns server.
- main file is in `src/main/dns/dns_client.cpp`

**dns_server**:
- dns server which will answer dns queries it receives.
- you can specify a database with various records to be loaded in to the server.
- format for the database file (there is a sample database in `src/main/dns/db.temp`:
```
<type> <key> <value>
<type> <key> <value>
<type> <key> <value>
...
```
- current supported types are: A, MX.

## monitors & analyzers

**bandwidth monitor**:

under `src/tools/bandwidth`, there is a file `bandwidth` which is a tools that counts the amount of "outgoing" and "incoming" packets. it essential takes a `sniffer` class and attaches to it two counters, one for outgoing packets and one for incoming packets. you can use this class to monitor the size of the network traffic in constant intervals

sample bandwidth monitor tool can be found under `src/main/bandwidth/bandwidth.cpp`

**nework analyzer**:

under `src/tools/analyzer`, there is a file `net_analyzer` which is a networking tool for displaying the network traffic. it essential takes a sniffer and attaches a handler to both outgoing and incoming packets, which would process the packets and display their protocol stack and also show a hexdump of the raw bytes.

sample tool that analyzes network traffic can be found under `src/main/net_analyzer/net_analyzer.cpp`


## remote shell

this project contains an implementation for a remote shell server & client tools. main files can be found under `src/main/remote_shell.cpp`

## netcat

this project contains an implementation for a simple "netcat" raw communication server & client tools. main files can be found under `src/main/netcat.cpp`

## scanners & interceptors

under `src/tools/net_arp`, there is a class `net_arp` which is a full arp scanner & spoofer. it can be used to:
* search for specific hosts in a local network
* scan entire subnets
* perform MITM arp spoofing attack, both for sniffing traffic and both for completely blocking traffic. attack can be done on specific hosts or entire broadcast
* announce new MAC address for a specific IP address
* and more...

a sample tool for scannig local network can be found under `src/main/net_scanner/net_scanner.cpp`

a sample tool for intercepting local network can be found under `src/main/net_intercept/net_intercept.cpp`

a sample tool for announcing and changing MAC address can be found under `src/main/net_intercept/mac_changer.cpp`

## vpn
under `src/tools/vpn` there is a simple implementation for a vpn which doesn't use encryption but merely boundary-preserving tcp connection:

**vpn_daemon**:

a vpn daemon to deploy on a local network. it listens to a vpn client, and upon connection establishment, sends all local traffic to the vpn client and all the client's traffic onto the local network.

a sample main file for using this tool can be found under `src/main/vpn/vpn_daemon_main.cpp`

**vpn_client**:

a vpn client which connects to a running daemon. upon connection establishment, sends its related traffic to the vpn daemon, and receives all the remove network's traffic.

a sample main file for using this tool, as well as changing the local routing table accordingly can be found under `src/main/vpn/vpn_client_main.cpp`

## onion network & proxies

under `src/tools/proxy`:

* `ip_proxy_server`: a general proxy server for ipv4. currently only supports a single client. it receives ip packets through the client's `connection` provided to it, stores internally the source address and forwards the packet in the guise of the source address of the server. when received back, the proxy searches for the correct source address and forwards the packet as with the corresponding address.
* `ip_proxy_client`: a general utility for a client to connect to a proxy server. takes any `connection`, and establishes the required connection to the server. (currently just does nothing, the server would receive any packet without premade authentication or handshake).

similar to a TOR network, this project contains an implementation for a simple onion network for ipv4 under `src/tools/onion_netowrk`:

**onion_network_node**:

server daemon to deploy. the machine running this server must be somehow connected to the ip network. this class is essentially a wrapper around `ip_proxy_server` which uses udp to listen to clients.

sample main file for using this tool can be found under `src/main/network/onion_network_node_main.cpp`

**onion_network_client**:

a client to connect to a path of nodes in the network. this class is a wrapper around `ip_proxy_client` which uses udp to connect to servers.

sample main file for using this tool can be found under `src/main/network/onion_network_client_main.cpp`

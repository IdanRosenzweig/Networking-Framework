# send and receive data
under `abstract/sending`:
* `send_medium`: pure virtual class, representing networking logic for sending data
* `multi_send_medium`: implementation of `send_medium` for sending data through multiple sub-`send_medium`s

under `abstract/receiving`:
* `recv_listener`: pure virtual class, representing networking logic for an actor which can handle received data
* `multi_recv_listener`: implementation of `recv_listener` for receiving data into multiple sub-`recv_listener`s

<!-- # connections
under `abstract/connection`:
* `two_way_connection`: a simple class which contains `send_medium` and `recv_listener`, representing two-way connection -->

# firewalls
under `abstract/firewall`:
* `basic_firewall_filter`: pure virtual class, representing a firewall rule which takes data and produces a `firewall_policy` decision for it
* `firewall_send`: implementation of `send_medium` for sending data through a sub-`send_medium` after applying filters
* `firewall_recv`: implementation of `recv_listener` for receiving data into a sub-`recv_listener` after applying filters

# sniffers
under `abstract/sniffer`:
* `basic_sniff_handler`: pure virtual class, handles callbacks for sniffed data
* `sniffer_send`: implementation of `send_medium` that passes sent data to sniffer handlers
* `sniffer_recv`: implementation of `recv_listener` taht passes received data to sniffer handlers
* `sniffer_net_access`: a wrapper implemenation of `net_access`, which takes a concrete `net_access` and hooks sniffer handlers for sent and received data

# network access
under `abstract/network_access`:
* `net_access`: pure virtual class providing a `send_medium` and the ability to specify a `recv_listener`, representing an logical network access where data can be sent to and received from
* `multi_net_access`: takes a concrete `net_access` and allows for creating new `net_access`s that all share the common `net_access`.
* `net_access_generator`: represents a class which generates `net_access`s to be used
* `net_access_generator_listener`: pure virtual class that handles new connections from a `net_access_generator`.

# routing
under `abstract/routing`:
* `routing_table`: pure virtual class representing a routing table, allows for adding routing rules and finding the correct route for addrs.
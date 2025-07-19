# send and receive data
under `abstract/sending`:
* `basic_send_medium`: pure virtual class, representing networking logic for sending data
* `multi_send_medium`: implementation of `basic_send_medium` for sending data through multiple sub-`basic_send_medium`s

under `abstract/receiving`:
* `basic_recv_listener`: pure virtual class, representing networking logic for an actor which can handle received data
* `multi_recv_listener`: implementation of `basic_recv_listener` for receiving data into multiple sub-`basic_recv_listener`s

<!-- # connections
under `abstract/connection`:
* `two_way_connection`: a simple class which contains `basic_send_medium` and `basic_recv_listener`, representing two-way connection -->

# firewalls
under `abstract/firewall`:
* `basic_firewall_filter`: pure virtual class, representing a firewall rule which takes data and produces a `firewall_policy` decision for it
* `firewall_send`: implementation of `basic_send_medium` for sending data through a sub-`basic_send_medium` after applying filters
* `firewall_recv`: implementation of `basic_recv_listener` for receiving data into a sub-`basic_recv_listener` after applying filters

# sniffers
under `abstract/sniffer`:
* `basic_sniff_handler`: pure virtual class, handles callbacks for sniffed data
* `sniffer_send`: implementation of `basic_send_medium` for sending data through a sub-`basic_send_medium` after passing it to sniffer handlers
* `sniffer_recv`: implementation of `basic_recv_listener` for receiving data into a sub-`basic_recv_listener` after passing it to sniffer handlers

# network access
under `abstract/network_access`:
* `network_access`: pure virtual class providing a `basic_send_medium` and the ability to specify a `basic_recv_listener`, representing an logical network access where data can be sent to and received from

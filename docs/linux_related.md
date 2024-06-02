# linux related functionalities and classes
under `src/linux/if`:
* `iface_access_point`: a class created around a linux interface. this provides various functionalities for the interface, including sniffing outoing & incoming packets and sending packest onto the interface
* `hardware`: functions for retrieving information about linux interfaces, including the MAC address, IP address and default gateway.

under `src/linux/if/virtual`:
* `tun_tap`: functions for opening TUN/TAP linux virtual interfaces
* `virtual_if`: a class that takes a `gateway` and opens a TAP interface round it; that is, data received from the `gateway` is forwarded to the TAP interface as "received from the wire", and data that is sent from the TAP interface is forwarded to be sent to the `gateway`

under `src/linux/if/wrappers`, there are wrapper classes around a `iface_access_point` that provide api for `gateway` (in `interface_gatway`) and `sniffer` (in `interface_sniffer`)

under `src/linux/osi`, there are wrapper classes around a `iface_access_point` that provide api for `gateway` in the sense of osi terms:
* `data_link_layer_gatway`: practically the same as the raw `interface_gateway`
* `network_layer_gateway`: takes a `data_link_layer` and uses ethernet to provide abstract network layer gateway, without having to worry about the data link layer protocols

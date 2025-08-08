# linux related functionalities and classes
under `lib/linux/if/virtual`:
* `tun_tap`: functions for opening TUN/TAP linux virtual interfaces
<!-- * `virtual_if`: a class that takes a `gateway` and opens a TAP interface round it; that is, data received from the `gateway` is forwarded to the TAP interface as "received from the wire", and data that is sent from the TAP interface is forwarded to be sent to the `gateway` -->
* `hardware`: functions for retrieving information about linux interfaces, including the MAC address, IP address and default gateway.
* `linux_iface`: a class created above a linux interface. it prodives a send access `send_medium` for sending data out to the interface, and provides the ability to specific a recv access `recv_listener` which will receive the data received at the interface
* `linux_iface_net_access`: a wrapper class around a `linux_iface` which provides network access functionality


<!-- 
under `lib/linux/if/wrappers`, there are wrapper classes around a `iface_access_point` that provide api for `gateway` (in `interface_gatway`) and `sniffer` (in `interface_sniffer`)

under `lib/linux/osi`, there are wrapper classes around a `iface_access_point` that provide api for `gateway` in the sense of osi terms:
* `data_link_layer_gatway`: practically the same as the raw `interface_gateway`
* `network_layer_gateway`: takes a `data_link_layer` and uses ethernet to provide abstract network layer gateway, without having to worry about the data link layer protocols -->

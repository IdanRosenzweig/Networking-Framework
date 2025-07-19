// #include "interface_sniffer.h"

// interface_sniffer::interface_sniffer(const weak_ptr<iface_access_point> &raw) : raw_interface(raw) {
//     if (shared_ptr<iface_access_point> ptr = raw_interface.lock()) {
//         ptr->outgoing_traffic.add_listener(&this->outgoing);
//         ptr->incoming_traffic.add_listener(&this->incoming);
//     }
// }

// interface_sniffer::~interface_sniffer() {
//     if (shared_ptr<iface_access_point> ptr = raw_interface.lock()) {
//         ptr->outgoing_traffic.remove_listener(&this->outgoing);
//         ptr->incoming_traffic.remove_listener(&this->incoming);
//     }
// }

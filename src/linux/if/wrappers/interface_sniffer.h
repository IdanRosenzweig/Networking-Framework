#ifndef NETWORKING_INTERFACE_SNIFFER_H
#define NETWORKING_INTERFACE_SNIFFER_H

#include "../../../abstract/sniffer/sniffer.h"
#include "../iface_access_point.h"

#include <memory>
using namespace std;

// wrapper class for the raw interface access point, providing sniffer api
class interface_sniffer : public sniffer {
   weak_ptr<iface_access_point> raw_interface;

public:
    interface_sniffer(const weak_ptr<iface_access_point>& raw) : raw_interface(raw) {
        if (shared_ptr<iface_access_point> ptr = raw_interface.lock()) {
            ptr->outgoing_traffic.add_listener(&this->outgoing);
            ptr->incoming_traffic.add_listener(&this->incoming);
        }
    }

    ~interface_sniffer() {
        if (shared_ptr<iface_access_point> ptr = raw_interface.lock()) {
            ptr->outgoing_traffic.remove_listener(&this->outgoing);
            ptr->incoming_traffic.remove_listener(&this->incoming);
        }
    }

};


#endif //NETWORKING_INTERFACE_SNIFFER_H

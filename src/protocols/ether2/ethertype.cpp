#include "ethertype.h"

optional<string> ethertype_to_str(ethertype prot) {
    switch (prot) {
        case ethertype::arp: return "arp";
        case ethertype::ip4: return "ip4";
        default: return {};
    }
}

optional<ethertype> str_to_ethertype(string const& str) {
    if (str == "arp") return ethertype::arp;
    else if (str == "ip4") return ethertype::ip4;
    else return {};
}
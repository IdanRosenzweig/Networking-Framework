#include "ip4_addr.h"
#include <arpa/inet.h>

ip4_addr convert_to_ip4_addr(const string& str) {
    return ip4_addr{inet_network(str.c_str())};
}

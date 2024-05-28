#ifndef SERVERCLIENT_TUN_TAP_H
#define SERVERCLIENT_TUN_TAP_H

#include <string>

// opens a TUN/TAP interface
int open_tun_tap(std::string& iface_name, int flags);

// opens a raw TAP interface (wrapper of open_tun_tap)
int open_raw_tap(std::string& iface_name);

void set_up_tun_tap(const std::string& iface_name);
void set_down_tun_tap(const std::string& iface_name);

#endif //SERVERCLIENT_TUN_TAP_H

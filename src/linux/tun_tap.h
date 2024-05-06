#ifndef SERVERCLIENT_TUN_TAP_H
#define SERVERCLIENT_TUN_TAP_H

// opens a TUN/TAP interface
int open_tun_tap(char *dev, int flags);

// opens a raw TAP interface (wrapper of open_tun_tap)
int open_raw_tap(char *dev);

void set_up_tun_tap(char *dev);
void set_down_tun_tap(char *dev);

#endif //SERVERCLIENT_TUN_TAP_H

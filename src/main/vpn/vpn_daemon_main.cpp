#include "../../temp_utils/vpn/vpn_daemon.h"
#include <string>

using namespace std;

void vpn_main() {
    string local_interface = "enp0s3";
    vpn_daemon vpnDaemon(local_interface);

    while (true) {}
}


int main() {
    vpn_main();
    return 0;
}
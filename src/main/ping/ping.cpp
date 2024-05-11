#include "../../temp_utils/ping/ping_util.h"

void traceroute_main(ip4_addr dest_ip, int count) {

    ping_util pinger;
    pinger.dest_ip.set_next_choice(dest_ip);
    pinger.count.set_next_choice(count);
    pinger.delay_interval.set_next_choice(1000ms);

    pinger.ping_node();

}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "usage: ping [dest_ip] [count]" << endl;
        return 1;
    }
    traceroute_main(convert_to_ip4_addr(argv[1]), std::stoi(argv[2]));

}
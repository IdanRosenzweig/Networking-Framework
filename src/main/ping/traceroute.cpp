#include "../../temp_utils//ping/traceroute_util.h"

void traceroute_main(ip4_addr dest_ip) {
    traceroute_util tracer;
    tracer.dest_ip.set_next_choice(dest_ip);

    tracer.trace_to_destination();

}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "give the dest ip as second argument" << endl;
        return 1;
    }
    traceroute_main(convert_to_ip4_addr(argv[1]));
}


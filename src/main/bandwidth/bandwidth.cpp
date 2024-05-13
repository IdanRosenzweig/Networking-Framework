#include "../../linux/interface_gateway.h"
#include "../../linux/interface_sniffer.h"
#include "../../temp_utils/bandwidth/bandwidth.h"
#include <unistd.h>

void bandwidth_main(const string& interface) {
    interface_sniffer sniffer("enp0s3");

    bandwidth indicator(&sniffer);

    while (true) {
        sleep(1);

        cout << "in: " << indicator.bytes_in_cnt << " B\t" << "out: " << indicator.bytes_out_cnt << " B" << endl;
        indicator.bytes_in_cnt = 0;
        indicator.bytes_out_cnt = 0;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "usage: bandwidth [interface name]" << endl;
        return 1;
    }
    bandwidth_main(argv[1]);
}
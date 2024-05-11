#include "../../temp_utils/dns_server_client/dns_client.h"
#include <unistd.h>

void dns_client_main(dns_record_type type, const string &key, ip4_addr dns_server) {
    dns_client client(dns_server);

    client.query(type, key);

}

int main(int argc, char **argv) {
    if (argc < 3) goto print_usage;

    if (argc == 3) {
        dns_client_main(str_to_record_type(argv[1]), argv[2], convert_to_ip4_addr("8.8.8.8"));
    } else if (argc == 4) {
        dns_client_main(str_to_record_type(argv[1]), argv[2], convert_to_ip4_addr(argv[3]));
    } else goto print_usage;

    return 0;

    print_usage:
    cout << "usage: dns_client type record_key [specific_server]" << endl;
    return 1;
}
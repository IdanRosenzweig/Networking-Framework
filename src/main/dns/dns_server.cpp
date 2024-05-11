#include "../../temp_utils/dns_server_client/dns_server.h"
#include "../../temp_utils/dns_server_client/database/database_loader.h"
#include <unistd.h>

void dns_server_main(const string& config_file) {
    dns_server server;

    load_config_database(&server, config_file);

    while (true) {
    }

}

int main(int argc, char** argv) {
    if (argc < 2) goto print_usage;

    dns_server_main(argv[1]);
    return 0;

    print_usage:
    cout << "usage: dns_server database_file" << endl;
    return 1;
}
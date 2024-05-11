#include "database_loader.h"

void add_a_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string ip;
    input >> ip;

    server->mappings_type_a.insert({domain,
                                    convert_to_ip4_addr(ip)
                                   });
}

void add_mx_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string server_domain;
    input >> server_domain;

    server->mappings_type_mx.insert({domain,
                                    server_domain
                                   });
}

void load_config_database(dns_server *server, const string& config_file) {
    std::ifstream input(config_file);
    while (true) {
        if (input.eof()) break;

        string type;
        input >> type;

        if (type == "a")
            add_a_record(server, input);
        else if (type == "mx")
            add_mx_record(server, input);
        else {
            cerr << "invalid type of dns record" << endl;
            break;
        }

    }
    cout << "finished loading dns config" << endl;
}
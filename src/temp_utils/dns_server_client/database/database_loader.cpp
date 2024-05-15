#include "database_loader.h"

#include <iostream>
using namespace std;

void add_a_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string ip;
    input >> ip;

    auto node = server->mappings_type_a.add_word(domain);
    node->key = convert_to_ip4_addr(ip);
}

void add_mx_record(dns_server *server, std::ifstream &input) {
    string domain;
    input >> domain;

    string server_domain;
    input >> server_domain;

    auto node = server->mappings_type_mx.add_word(domain);
    node->key = server_domain;
}

void load_database(dns_server *server, const string& db_path) {
    std::ifstream input(db_path);
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
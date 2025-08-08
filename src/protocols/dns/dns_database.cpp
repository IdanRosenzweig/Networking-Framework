#include "dns_database.h"

#include <sstream>
#include <iostream>

void print_hostname_ip_pairs(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string hostname;
    std::string ip_address;
    
    // Read the file line by line
    while (infile >> hostname >> ip_address) {
        std::cout << "Hostname: " << hostname << ", IP Address: " << ip_address << std::endl;
    }

    infile.close();
}


vector<pair<string, ip4_addr>> dns_parse_records_a(string const& file) {
    vector<pair<string, ip4_addr>> res;

    std::ifstream input(file);
    if (!input.is_open()) return {};

    string name;
    string ip_str;
    
    while (input >> name >> ip_str) {
        ip4_addr addr;
        if (auto tmp = str_to_ip4_addr(ip_str); tmp.has_value()) addr = tmp.value();
        else continue;

        res.push_back({name, addr});
    }

    input.close();

    return res;
}

vector<pair<string, string>> dns_parse_records_mx(string const& file) {
    vector<pair<string, string>> res;

    std::ifstream input(file);
    if (!input.is_open()) return {};

    string name;
    string val;
    
    while (input >> name >> val) {
        res.push_back({name, val});
    }

    input.close();

    return res;
}


void dns_database::add_record_a(string const& name, ip4_addr addr) {
    mappings_a.add_word(name)->key = addr;
}

optional<ip4_addr> dns_database::search_record_a(string const& name) {
    auto res = mappings_a.search(name);
    if (!(res != nullptr && res->marked)) return {};
    
    return res->key;
}

void dns_database::add_record_mx(string const& name, string const& res) {
    mappings_mx.add_word(name)->key = res;
}

optional<string> dns_database::search_record_mx(string const& name) {
    auto res = mappings_mx.search(name);
    if (!(res != nullptr && res->marked)) return {};
    
    return res->key;
}
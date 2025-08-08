#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

#include "src/protocols/ip4/ip4_addr.h"

#include "src/utils/trie.h"

vector<pair<string, ip4_addr>> dns_parse_records_a(string const& data);

vector<pair<string, string>> dns_parse_records_mx(string const& data);

struct dns_database {
private:
    static constexpr auto assign = [](char val) -> size_t {
        return (size_t) val;
    };
    trie_node<char, 256, assign, ip4_addr> mappings_a;
    trie_node<char, 256, assign, string> mappings_mx;
 
public:
    void add_record_a(string const& name, ip4_addr addr);
    optional<ip4_addr> search_record_a(string const& name);

    void add_record_mx(string const& name, string const& res);
    optional<string> search_record_mx(string const& name);
};
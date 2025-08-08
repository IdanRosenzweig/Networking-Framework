#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

#include "src/abstract/net_access/net_access.h"

#include "src/protocols/dns/dns.h"
#include "src/protocols/dns/dns_database.h"

namespace dns_tool {
    void send_requests(shared_ptr<net_access> const& dns_surface, struct dns_query const& query);

    struct dns_answer get_my_start_of_authority();

    struct dns_message handle_query(dns_database* database, struct dns_message const* msg);
}

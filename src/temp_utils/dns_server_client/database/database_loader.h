#ifndef SERVERCLIENT_DATABASE_LOADER_H
#define SERVERCLIENT_DATABASE_LOADER_H

#include "../dns_server.h"
#include <string>
using namespace std;

void load_config_database(dns_server* server, const string& configfile);

#endif //SERVERCLIENT_DATABASE_LOADER_H

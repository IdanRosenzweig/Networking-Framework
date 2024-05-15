#ifndef SERVERCLIENT_DATABASE_LOADER_H
#define SERVERCLIENT_DATABASE_LOADER_H

#include "../dns_server.h"
#include <string>
using namespace std;

void load_database(dns_server* server, const string& db_path);

#endif //SERVERCLIENT_DATABASE_LOADER_H

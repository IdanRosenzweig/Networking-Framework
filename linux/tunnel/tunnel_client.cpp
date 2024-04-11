#include "tunnel_client.h"

using namespace std;

int tunnel_client::recv_data(void* data, int count) {
    return client->recv_data(data, count);
}

int tunnel_client::send_data(void* data, int cnt) {
    return client->send_data(data, cnt);
}
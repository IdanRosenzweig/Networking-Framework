#include "ssh_conn_session.h"

int ssh_conn_session::send_data(send_msg val) {
    return ssh_channel_write(raw_channel, val.buff, val.count);
}



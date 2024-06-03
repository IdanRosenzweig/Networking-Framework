#include "vpn_daemon.h"

vpn_daemon::sessions_handler::sessions_handler(vpn_daemon *_master) : master(_master) {
    master->tcpServer.set_generator_listener(this);
}

void vpn_daemon::sessions_handler::handle_callback(tcp_session_type &&data) {
    master->tcpSession.push_back(std::move(data));

//            master->sessions.emplace_back(master->tcpSession.back().sess_conn.get());
//            master->aggregator.add_connection(&master->sessions.back());
    master->sessions.emplace_back(make_unique<msg_boundary_seperator<>>(master->tcpSession.back().sess_conn.get()));
    master->aggregator.add_connection(master->sessions.back().get());
}

vpn_daemon::vpn_daemon(const weak_ptr<iface_access_point> &access) : dataLinkLayerGateway(access),
                                                                     gateway_firewall(&dataLinkLayerGateway), block_filter(VPN_DAEMON_PORT),
                                                                     tcpServer(VPN_DAEMON_PORT, access.lock()->iface_name), handler(this) {
    gateway_firewall.incoming_filters.push_back(&block_filter);
    aggregator.add_connection(&gateway_firewall);
}

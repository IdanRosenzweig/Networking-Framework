//#ifndef SERVERCLIENT_ARP_CONN_CLIENT_H
//#define SERVERCLIENT_ARP_CONN_CLIENT_H
//
//#include <string>
//#include <vector>
//
//#include "../ether/ethernet_conn_client.h"
//
//class arp_conn_client  {
//public:
//    arp_conn_client();
//
//    ethernet_conn_client* ether_client;
//
//    mac_addr search_for_device(std::string priv_ip);
//
//    // spoof the victim devices into thinking that the private ip addr
//    // is associated with our mac addr (empty victim list means broadcast)
//    void spoof_as_device(std::string device, std::vector<std::pair<mac_addr, std::string>>& victim_devices, bool block_traffic);
//
//};
//
//#endif //SERVERCLIENT_ARP_CONN_CLIENT_H

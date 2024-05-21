#ifndef SERVERCLIENT_FIREWALL_POLICY_H
#define SERVERCLIENT_FIREWALL_POLICY_H

enum firewall_policy {
    BLOCK, // block packet
    ALLOW, // allow packet to move
    DELAY // delay packet
};

#endif //SERVERCLIENT_FIREWALL_POLICY_H

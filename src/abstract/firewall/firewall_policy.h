#ifndef NETWORKING_FIREWALL_POLICY_H
#define NETWORKING_FIREWALL_POLICY_H

enum firewall_policy {
    BLOCK, // block packet
    ALLOW, // allow packet to move
    DELAY // delay packet
};

#endif //NETWORKING_FIREWALL_POLICY_H

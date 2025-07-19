#pragma once

enum class firewall_policy {
    BLOCK, // block packet
    ALLOW, // allow packet to move
    DELAY // delay packet
};

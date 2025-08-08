#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <vector>
#include <memory>
using namespace std;

/* sniffed packet */
enum class net_analyzer_capture_dir {
    outgoing, // outoing data
    incoming, // incoming data
    // self_incoming // incoming data, came from local
};

struct net_analyzer_sniffed_packet {
    chrono::time_point<chrono::system_clock> time;
    vector<uint8_t> data;
};
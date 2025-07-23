#pragma once

#include "net_access.h"

#include <vector>
#include <cstdint>
using namespace std;

// net access for raw bytes
using net_access_bytes = net_access<vector<uint8_t>, vector<uint8_t>>;
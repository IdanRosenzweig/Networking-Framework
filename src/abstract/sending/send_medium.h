#pragma once

#include <cstdint>
#include <vector>
using namespace std;

template <typename TYPE>
class send_medium {
public:
    enum class err_t {OK, ERR};
    virtual err_t send_data(TYPE const& data) = 0;

    virtual ~send_medium() = default;
};

using send_medium_bytes = send_medium<vector<uint8_t>>;
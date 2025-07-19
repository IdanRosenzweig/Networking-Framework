#pragma once

template <typename TYPE>
class basic_send_medium {
public:
    enum class err_t {OK, ERR};
    virtual err_t send_data(TYPE const& data) = 0;

    virtual ~basic_send_medium() = default;
};

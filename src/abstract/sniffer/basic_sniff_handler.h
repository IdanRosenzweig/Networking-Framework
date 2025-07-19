#pragma once

template <typename T>
struct basic_sniff_handler {
    virtual void handle_sniff(T const& val) = 0;
};

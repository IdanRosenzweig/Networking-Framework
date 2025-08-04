#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

size_t cnt_digits_base(size_t num, size_t base);

string padded_hex_num(size_t num, size_t no_digits);

string produce_hexdump(vector<uint8_t> const& data, size_t row_sz, bool show_ascii);

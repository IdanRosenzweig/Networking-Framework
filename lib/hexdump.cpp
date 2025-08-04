#include "hexdump.h"

size_t cnt_digits_base(size_t num, size_t base) {
    if (num == 0) return 1;
    
    long double log_16 = (long double) log((long double) num) / (long double) log((long double) base);
    return static_cast<size_t>(floor(log_16)) + 1;
}

string padded_hex_num(size_t num, size_t no_digits) {
    stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(no_digits);
    
    ss << num;

    return ss.str();
}

string produce_hexdump(vector<uint8_t> const& data, size_t row_sz, bool show_ascii) {
    string res;

    size_t cnt = data.size(); // total amount of bytes
    size_t no_rows = cnt / row_sz + (cnt % row_sz != 0); // number of rows in the result

    size_t no_digits = cnt_digits_base(cnt / row_sz * row_sz, 16);

    for (size_t row = 0; row < no_rows; row++) {
        res += "0x";
        res += padded_hex_num(row_sz * row, no_digits);
        res += ": ";

        for (size_t i = 0; i < row_sz; i++) {
            if (i > 0) res += ' ';

            if (!(row_sz * row + i < cnt)) {
                res += "  ";
                continue;
            }

            static stringstream ss;

            ss << std::hex << std::setfill('0') << std::setw(2);
            ss << static_cast<int>(data[row_sz * row + i]);

            res += ss.str();

            // reset stream
            ss.str("");
            ss.clear();
        }

        if (show_ascii) {
            res += " ";

            res += '|';
            for (size_t i = 0; i < row_sz; i++) {
                if (!(row_sz * row + i < cnt)) {
                    res += ' ';
                    continue;
                }
            
                if (std::isprint(static_cast<unsigned char>(data[row_sz * row + i]))) res += static_cast<char>(data[row_sz * row + i]);
                else res += '.';
            }
            res += '|';
        }

        if (row + 1 < no_rows) res += '\n';
    }

    return res;
}
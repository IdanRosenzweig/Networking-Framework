#include <netinet/in.h>
#include "dns.h"
#include <iostream>

ustring encode_dns_name(uint8_t *name) {
    ustring encoded;

    int name_len = strlen((char *) name);

    int last = 0;
    int curr = 0;
    while (curr < name_len) {

        if (name[curr] == '.') {
            int len = curr - last;
            encoded += (uint8_t) len;

            for (int j = last; j < curr; j++) {
                encoded += name[j];
            }

            curr++;
            last = curr;

        } else
            curr++;
    }

    int len = curr - last;
    if (len != 0) {
        encoded += (uint8_t ) len;
        for (int i = last; i < name_len; i++) {
            encoded += name[i];
        }
    }

    encoded += (uint8_t) 0; // ending with null byte

    return encoded;
}

void decode_dns_name(ustring &name) {
    int i = 0;
    while (i < name.size()) {
        int len = name[i];

        int j = 0;
        while (j < len) {
            name[i] = name[i + 1];
            j++;
            i++;
        }
        name[i] = '.';

        i++;
    }
    if (name[name.size() - 1] == '.')
        name.pop_back();
}

int extract_encoded_name(ustring *dest, uint8_t *curr_ptr, uint8_t *packet_buff) {
    int count = 0;

    // extract the encoded name from the packet
    bool jumped = false;
    while (*curr_ptr != 0) {

#define JUMP_MASK (0b11000000)
        if ((*curr_ptr & JUMP_MASK) == JUMP_MASK) { // this is a jump

            int offset = (((*curr_ptr) ^ JUMP_MASK) << 8) + *(curr_ptr + 1); // the value (number is in big endian)

            curr_ptr = packet_buff + offset;
            jumped = true;

        } else {
            *dest += *curr_ptr;
            curr_ptr++;

            if (!jumped) // we are not after a jump, increment the count
                count++;
//            else // we are after a jump, this byte is not stored in our current answer space
        }
    }

    // update the final value of count
    if (jumped) // string ended AFTER we jumped, meaning additional 2 bytes for the offset used in the jump
        count += 2;
    else // string ended without a jump, add 1 byte for the trailing null byte
        count += 1;

    return count;
    return count;
}

int extract_query(struct dns_query *dest, uint8_t *curr_ptr, uint8_t *packet_buff) {
    int cnt = 0;

    cnt += extract_encoded_name(&dest->q_name, curr_ptr + cnt, packet_buff);
    decode_dns_name(dest->q_name);

    dest->q_type = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->q_type);

    dest->q_class = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->q_class);

    return cnt;
}

int extract_response(struct dns_answer *dest, uint8_t *curr_ptr, uint8_t *packet_buff) {
    int cnt = 0;

    cnt += extract_encoded_name(
            &dest->name, curr_ptr + cnt, packet_buff
    );
    decode_dns_name(dest->name);

    dest->type = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->type);

    dest->_class = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->_class);

    dest->ttl = ntohs(*(uint32_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->ttl);

    dest->data_len = ntohs(*(uint16_t *) (curr_ptr + cnt));
    cnt += sizeof(dest->data_len);

    for (int j = 0; j < dest->data_len; j++)
        dest->rdata += (curr_ptr + cnt)[j];
    cnt += dest->data_len;

    return cnt;
}

int write_query_to_buff(uint8_t *dest, struct dns_query *source) {
    int cnt = 0;

    source->q_name = encode_dns_name((uint8_t*) source->q_name.c_str());
    memcpy(dest + cnt, source->q_name.c_str(), source->q_name.size());
    cnt += source->q_name.size();

    *(uint16_t *) (dest + cnt) = htons(source->q_type);
    cnt += sizeof(source->q_type);

    *(uint16_t *) (dest + cnt) = htons(source->q_class);
    cnt += sizeof(source->q_class);

    return cnt;
}

int write_response_to_buff(uint8_t *dest, struct dns_answer *source) {
    int cnt = 0;

    ustring encoded_name = encode_dns_name((uint8_t *) source->name.c_str());
    memcpy(dest + cnt, encoded_name.c_str(), encoded_name.size());
    cnt += encoded_name.size();

    *(uint16_t *) (dest + cnt) = htons(source->type);
    cnt += sizeof(source->type);

    *(uint16_t *) (dest + cnt) = htons(source->_class);
    cnt += sizeof(source->_class);

    *(uint32_t *) (dest + cnt) = htons(source->ttl);
    cnt += sizeof(source->ttl);

    *(uint16_t *) (dest + cnt) = htons(source->data_len);
    cnt += sizeof(source->data_len);

    for (int i = 0; i < source->data_len; i++) {
        (dest + cnt)[i] = source->rdata[i];
    }
    cnt += source->data_len;
//    switch (source->type) {
//        case DNS_TYPE_A: {
//            for (int i = 0; i < source->data_len; i++) {
//                (dest + cnt)[i] = source->rdata[i];
//            }
//            cnt += source->data_len;
//            break;
//        }
//        case DNS_TYPE_CNAME: {
//            memcpy(dest + cnt, source->rdata.c_str(), source->rdata.size());
//            cnt += source->rdata.size();
//
//            break;
//        }
//    }

    return cnt;
}


dns_record_type str_to_record_type(const string& str) {
    if (str == "A") return DNS_TYPE_A;
    else if (str == "MX") return DNS_TYPE_MX;
    else if (str == "PTR") return DNS_TYPE_PTR;
    else {
        throw "invalid record type";
    }
}
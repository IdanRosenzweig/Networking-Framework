#ifndef SERVERCLIENT_IP4_HEADER_H
#define SERVERCLIENT_IP4_HEADER_H

struct ip4_header {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int hl: 4, /* header length */
    v: 4; /* version */
#else
    unsigned int v : 4, /* version */
                 hl : 4; /* header length */
#endif
    unsigned char tos; /* type of service */
    unsigned short len; /* total length */
    unsigned short id; /* identification */
    unsigned short off; /* fragment offset field */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
    unsigned char ttl; /* time to live */
    unsigned char p; /* protocol */
    unsigned short sum; /* checksum */
    struct in_addr src, dst; /* source and dest address */
};

#endif //SERVERCLIENT_IP4_HEADER_H

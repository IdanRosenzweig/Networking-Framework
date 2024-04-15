#ifndef SERVERCLIENT_CHECKSUM_H
#define SERVERCLIENT_CHECKSUM_H

unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

#endif //SERVERCLIENT_CHECKSUM_H

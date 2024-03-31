#ifndef SERVERCLIENT_ADDIT_DATA_H
#define SERVERCLIENT_ADDIT_DATA_H

struct prot_addit_data {
    // the only thing we need to store is the fd of the opened linux socket
    int fd{};

};

#endif //SERVERCLIENT_ADDIT_DATA_H

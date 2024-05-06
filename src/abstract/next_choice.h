#ifndef SERVERCLIENT_NEXT_CHOICE_H
#define SERVERCLIENT_NEXT_CHOICE_H

// generic class for choosing the next option (could be next protocol to receive, next tcpSession to send msg to, next port to listen on...)
template <typename CHOICE>
struct next_choice {
private:
    CHOICE next_choice;

public:
    CHOICE& get_next_choice() {return next_choice;}

    void set_next_choice(CHOICE choice) {next_choice = choice;}

};


#endif //SERVERCLIENT_NEXT_CHOICE_H

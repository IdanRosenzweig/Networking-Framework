#ifndef NETWORKING_NEXT_CHOICE_H
#define NETWORKING_NEXT_CHOICE_H

// generic class for choosing the next option (could be next protocol to receive, next raw_session to send msg to, next port to listen on...)
template <typename CHOICE>
struct next_choice {
private:
    CHOICE next_choice;

public:
    CHOICE& get_next_choice() {return next_choice;}

    void set_next_choice(CHOICE choice) {next_choice = choice;}

};


#endif //NETWORKING_NEXT_CHOICE_H

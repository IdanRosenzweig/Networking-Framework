#include "packet_displayer.h"
#include <iostream>
using namespace std;

void packet_displayer::handle_received_event(received_msg& event) {
    cout << "___PACKET START___" << endl;
    for (const auto& prot : event.protocol_offsets) {
        cout << "protocol: " << to_str(prot.second) << ", at offset: " << prot.first << endl;
    }
    cout << "___PACKET END___" << endl << endl;
}

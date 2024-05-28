#include "interface_sniffer.h"
#include <iostream>
using namespace std;

void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    interface_sniffer *sniffer = reinterpret_cast<interface_sniffer *>(user);

    int data_sz = pkthdr->caplen;
    if (data_sz <= 0) {
//        cerr << "pcap recieved size " << data_sz << endl;
        return;
    }
//    cout << "pcap recieved size " << data_sz << endl;

    received_msg msg;
    msg.data = udata_t(data_sz, 0x00);
    memcpy(msg.data.data(), bytes, data_sz);
    msg.curr_offset = 0;
    sniffer->handle_incoming_packet(msg);
}

void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    interface_sniffer *sniffer = reinterpret_cast<interface_sniffer *>(user);

    int data_sz = pkthdr->caplen;
    if (data_sz <= 0) {
//        cerr << "pcap recieved size " << data_sz << endl;
        return;
    }
//    cout << "pcap recieved size " << data_sz << endl;

    received_msg msg;
    msg.data = udata_t(data_sz, 0x00);
    memcpy(msg.data.data(), bytes, data_sz);
    msg.curr_offset = 0;
    sniffer->handle_outgoing_packet(msg);
}

interface_sniffer::interface_sniffer(const string& interface) {

    char errbuf[PCAP_ERRBUF_SIZE];

    // traffic in
    {
        traffic_in = pcap_open_live(interface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_in == nullptr) {
            std::cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
            throw;
        }

        const char *filter_exp = "inbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        worker_in = std::thread([this]() {
            pcap_loop(traffic_in, -1, handler_in, reinterpret_cast<u_char *>(this));
        });
    }
//    {
//        traffic_in = pcap_create(interface.c_str(), errbuf);
//        if (traffic_in == nullptr) {
//            std::cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
//            throw;
//        }
//
//        if (pcap_set_immediate_mode(traffic_in, 1) != 0) {
//            std::cerr << "pcap_set_immediate_mode() failed: " << pcap_geterr(traffic_in) << std::endl;
//            pcap_close(traffic_in);
//            return;
//        }
//
//        // Step 3: Set the timeout to 10 ms (optional, since immediate mode should bypass this)
////        if (pcap_set_timeout(traffic_in, 10) != 0) {
////            std::cerr << "pcap_set_timeout() failed: " << pcap_geterr(traffic_in) << std::endl;
////            pcap_close(traffic_in);
////            return;
////        }
//
//        // Step 4: Activate the handle
//        if (pcap_activate(traffic_in) != 0) {
//            std::cerr << "pcap_activate() failed: " << pcap_geterr(traffic_in) << std::endl;
//            pcap_close(traffic_in);
//            return ;
//        }
//
//
//        const char *filter_exp = "inbound";
//
//        struct bpf_program fp;
//        if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
//            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
//            return;
//        }
//
//        if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
//            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
//            return;
//        }
//
//
//        // Step 5: Start the packet capture loop
//        worker_in = std::thread([this]() {
//            if (pcap_loop(traffic_in, -1, handler_in, reinterpret_cast<u_char *>(this)) < 0) {
//                std::cerr << "pcap_loop() failed: " << pcap_geterr(traffic_in) << std::endl;
//                pcap_close(traffic_in);
//                return;
//            }
//        });
//    }


    // traffic out
    {
        traffic_out = pcap_open_live(interface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_out == nullptr) {
            std::cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
            throw;
        }

        const char *filter_exp = "outbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_out, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        if (pcap_setfilter(traffic_out, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        worker_out = std::thread([this]() {
            pcap_loop(traffic_out, -1, handler_out, reinterpret_cast<u_char *>(this));
        });
    }
//    {
//        traffic_out = pcap_create(interface.c_str(), errbuf);
//        if (traffic_out == nullptr) {
//            std::cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
//            throw;
//        }
//
//        if (pcap_set_immediate_mode(traffic_out, 1) != 0) {
//            std::cerr << "pcap_set_immediate_mode() failed: " << pcap_geterr(traffic_out) << std::endl;
//            pcap_close(traffic_out);
//            return;
//        }
//
//        // Step 3: Set the timeout to 10 ms (optional, since immediate mode should bypass this)
////        if (pcap_set_timeout(traffic_out, 10) != 0) {
////            std::cerr << "pcap_set_timeout() failed: " << pcap_geterr(traffic_out) << std::endl;
////            pcap_close(traffic_out);
////            return;
////        }
//
//        // Step 4: Activate the handle
//        if (pcap_activate(traffic_out) != 0) {
//            std::cerr << "pcap_activate() failed: " << pcap_geterr(traffic_out) << std::endl;
//            pcap_close(traffic_out);
//            return ;
//        }
//
//
//        const char *filter_exp = "outbound";
//
//        struct bpf_program fp;
//        if (pcap_compile(traffic_out, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
//            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
//            return;
//        }
//
//        if (pcap_setfilter(traffic_out, &fp) == PCAP_ERROR) {
//            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
//            return;
//        }
//
//
//        // Step 5: Start the packet capture loop
//        worker_out = std::thread([this]() {
//            if (pcap_loop(traffic_out, -1, handler_in, reinterpret_cast<u_char *>(this)) < 0) {
//                std::cerr << "pcap_loop() failed: " << pcap_geterr(traffic_out) << std::endl;
//                pcap_close(traffic_out);
//                return;
//            }
//        });
//    }

//    std::this_thread::sleep_for(2000ms); // ensure the pcap_loop has started

}

interface_sniffer::~interface_sniffer() {
    worker_in.detach();
    worker_out.detach();
}
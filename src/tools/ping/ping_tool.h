#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <iostream>
using namespace std;

#include "src/protocols/icmp/icmp_header.h"
#include "src/protocols/icmp/icmp_protocol.h"

#include "src/abstract/receiving/recv_queue.h"

namespace ping_tool {
    template <typename DURATION_T>
    void ping(shared_ptr<net_access_bytes> const& net_access, optional<size_t> cnt, DURATION_T const& delay_interval) {
        // listen to icmp messages via a queue
        shared_ptr<recv_queue<pair<icmp_header, vector<uint8_t>>>> queue = make_shared<recv_queue<pair<icmp_header, vector<uint8_t>>>>();
        icmp_protocol::connect_recv(net_access, queue, {});

        // prepare base icmp echo
        struct icmp_header icmp_header;
        
        icmp_header.type = icmp_type::echo;
        icmp_header.code = 0;

        icmp_header.content.echo.id = (uint16_t) 0x1234;
        icmp_header.content.echo.sequence = (uint16_t) 10;
        
        // send echos
        for (size_t i = 0; cnt.has_value() ? (i < cnt.value()) : true; i++) {
            auto start = std::chrono::high_resolution_clock::now(); // count start time

            queue->queue.clear();
            while (true) {
                // send icmp echo
                puts("sending icmp echo");
                icmp_protocol::send(net_access, icmp_header, vector<uint8_t>());

                // recv reply
                puts("waiting for reply");
                auto* reply = queue->queue.front(std::chrono::milliseconds(50)); // todo be able to control this value
                if (reply == nullptr) continue; // no response in a reasonable time, continue and send again
                queue->queue.pop_front();

                struct icmp_header icmp_reply = reply->first;

                // take timestamp
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = end - start;

                // handle response
                bool failed = false;
                switch (icmp_reply.type) {
                    case icmp_type::time_exceeded: {
                        std::cout << "received time Exceeded" << std::endl;
                        failed = true;
                        break;
                    }
                    case icmp_type::dest_unreachable: {
                        std::cout << "received destination unreachable" << std::endl;
                        failed = true;
                        break;
                    }
                    case icmp_type::redirect: {
                        std::cout << "received redirect" << std::endl;
                        failed = true;
                        break;
                    }
                    case icmp_type::echoreply: {
                        if (icmp_reply.content.echo.id != icmp_header.content.echo.id) {
                            std::cout << "received reply with a different id" << std::endl;
                            failed = true;
                            break;
                        }

                        std::cout << "received reply"
                                << ", seq_num=" << icmp_header.content.echo.sequence
                                << ", delay=" << chrono::duration_cast<chrono::milliseconds>(duration).count() << " millisecs"
                                << std::endl;
                        break;
                    }
                    default: {
                        std::cout << "received unknown type of icmp" << std::endl;
                        failed = true;
                        break;
                    }
                }

                if (!failed) break;
            }

            // advance the sequence number
            ++icmp_header.content.echo.sequence;

            // wait delay interval
            std::this_thread::sleep_for(delay_interval);
        }
    }

}

// class ping_util {
//     gateway *network_layer_gw;
//     icmp_protocol_stack prot_stack;
//     recv_queue<recv_msg_t> replies_recv_q;

// public:
//     ping_util(ip4_addr src_ip, gateway* gw);

//     ~ping_util();

//     next_choice<ip4_addr> dest_ip;
//     next_choice<int> count;
//     next_choice<std::chrono::duration<uint64_t, milli>> delay_interval;

//     void ping_node();

// };


#include "parsed_packet.h"

#include "src/protocols/ether2/ethernet2_header.h"
#include "src/protocols/ether2/ethernet2_protocol.h"

#include "src/protocols/arp/arp_header.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/protocols/ip4/ip4_addr.h"
#include "src/protocols/ip4/ip4_protocol.h"

#include "lib/hexdump.h"

string net_analyzer_prot_to_str(net_analyzer_prot prot) {
    switch (prot) {
        case net_analyzer_prot::ethernet2: return "ethernet2";
        case net_analyzer_prot::arp: return "arp";
        case net_analyzer_prot::ip4 : return "ip4";
        case net_analyzer_prot::udp: return "udp";
        case net_analyzer_prot::tcp: return "tcp";
        case net_analyzer_prot::icmp: return "icmp";
        case net_analyzer_prot::dns: return "dns";
        case net_analyzer_prot::mdns: return "mdns";
    }

    return "<err>";
}

optional<net_analyzer_prot> str_to_net_analyzer_prot(string const& str) {
    if (str == "ethernet2") return net_analyzer_prot::ethernet2;
    else if (str == "arp") return net_analyzer_prot::arp;
    else if (str == "ip4") return net_analyzer_prot::ip4;
    else if (str == "udp") return net_analyzer_prot::udp;
    else if (str == "tcp") return net_analyzer_prot::tcp;
    else if (str == "icmp") return net_analyzer_prot::icmp;
    else if (str == "dns") return net_analyzer_prot::dns;
    else if (str == "mdns") return net_analyzer_prot::mdns;
    else return {};
}


static vector<unique_ptr<net_analyzer_parsed_prot>> parse_prots(vector<uint8_t> const& data) {
    // instantiations
    local_ctx ctx;
    ctx.recv_ether2 = make_shared<my_recv_ether2>(&ctx);
    ctx.recv_arp = make_shared<my_recv_arp>(&ctx);
    ctx.recv_ip4 = make_shared<my_recv_ip4>(&ctx);

    // a tmp net access
    struct tmp_net_access : public net_access {
        shared_ptr<recv_listener_bytes> the_recv;

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
    };
    shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();

    // the entry point (the first protocol)
    ethernet2_protocol::connect_recv(tmp_access, ctx.recv_ether2, {}, {}, {});

    // start the flow
    tmp_access->the_recv->handle_recv(data);

    // return the result
    return std::move(ctx.res);
}

net_analyzer_parsed_packet parse_sniffed_packet(shared_ptr<net_analyzer_sniffed_packet> const& sniffed_packet) {
    net_analyzer_parsed_packet packet;

    packet.sniffed_packet = sniffed_packet;
    packet.prots = parse_prots(sniffed_packet->data);
    packet.desc = "some packet";

    return packet;
}

void print_parsed_packet(net_analyzer_parsed_packet const& packet) {
    constexpr size_t BORDER_LEN = 40;
    string border(BORDER_LEN, '#');

    // upper border
    cout << border << endl;

    // time
    std::time_t as_time_t = chrono::system_clock::to_time_t(packet.sniffed_packet->time);
    cout << std::ctime(&as_time_t);

    // capture dir
    // switch (packet.sniffed_packet->capture_dir) {
    //     case net_analyzer_capture_dir::outgoing: {
    //         cout << "outgoing" << endl;
    //         break;
    //     }
    //     case net_analyzer_capture_dir::incoming: {
    //         cout << "incoming" << endl;
    //         break;
    //     }
    // }

    // desc
    cout << packet.desc << endl;

    // prot stack
    for (auto const& entry : packet.prots) {
        // status
        char status;
        switch (entry->status) {
            case net_analyzer_parsed_status::valid: {
                status = 'v';
                break;
            }
            case net_analyzer_parsed_status::corrupted: {
                status = 'x';
                break;
            }
            case net_analyzer_parsed_status::untested: {
                status = '-';
                break;
            }
            case net_analyzer_parsed_status::unknown: {
                status = '?';
                break;
            }
        }
        cout << "(" << status << ")";

        // prot
        cout << " ";
        if (entry->prot.has_value()) cout << net_analyzer_prot_to_str(entry->prot.value());
        else cout << "prot_unknown";

        // desc
        cout << " ";
        cout << entry->desc;

        // end of line
        cout << endl;
    }

    // hex dump
    cout << produce_hexdump(packet.sniffed_packet->data, 16, true);
    cout << endl;

    // bottom border
    cout << border << endl;

}


void my_recv_ether2::handle_recv(pair<ethernet2_header, vector<uint8_t>> const& data) {
    // create ether2 desc
    string desc;

    desc += "src: ";
    desc += mac_addr_to_str(data.first.src_addr);

    desc += ", ";
    desc += "dest: ";
    desc += mac_addr_to_str(data.first.dest_addr);

    desc += ", ";
    desc += "ethertype: ";
    if (auto str = ethertype_to_str(data.first.prot); str.has_value()) desc += str.value();
    else desc += "<unsupported>";

    // push parsed prot
    ctx->res.push_back(make_unique<net_analyzer_parsed_prot>(
        net_analyzer_parsed_status::valid,
        net_analyzer_prot::ethernet2,
        desc
    ));

    switch (data.first.prot) {
        case ethertype::arp: {
            struct tmp_net_access : public net_access {
                shared_ptr<recv_listener_bytes> the_recv;

            protected:
                shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
            };
            shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();
            
            arp_protocol::connect_recv(tmp_access, ctx->recv_arp, {});

            tmp_access->the_recv->handle_recv(data.second);

            break;
        }
        case ethertype::ip4: {
            struct tmp_net_access : public net_access {
                shared_ptr<recv_listener_bytes> the_recv;

            protected:
                shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
            };
            shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();
            
            ip4_protocol::connect_recv(tmp_access, ctx->recv_ip4, {}, {}, {});

            tmp_access->the_recv->handle_recv(data.second);

            break;
        }
    }
}

void my_recv_arp::handle_recv(arp_header const& data) {
    // create arp desc
    string desc;

    if (auto str = ethertype_to_str(data.prot_type); str.has_value()) desc += str.value();
    else desc += "<unsupported>";

    desc += " -> ";

    // if (auto str = ethertype_to_str(data.hard_type); str.has_value()) desc += str.value();
    // else desc += "<unsupported>";
    desc += "mac"; // todo check the hard type as well

    desc += ", ";
    switch (data.op) {
        case arp_op_values::request: {
            desc += "request for ";
            switch (data.prot_type) {
                case ethertype::ip4: {
                    ip4_addr requested_ip;
                    extract_from_network_order(&requested_ip, data.target_prot_addr.data());
                    desc += ip4_addr_to_str(requested_ip);
                    break;
                }
                default: {
                    desc += "<unknown>";
                    break;
                }
            }
            break;

        }
        case arp_op_values::reply: {
            desc += "reply: ";

            switch (data.prot_type) {
                case ethertype::ip4: {
                    ip4_addr reply_ip;
                    extract_from_network_order(&reply_ip, data.sender_prot_addr.data());
                    desc += ip4_addr_to_str(reply_ip);
                    break;
                }
                default: {
                    desc += "<unknown>";
                    break;
                }
            }

            desc += " is ";

            mac_addr reply_mac{}; // todo check the hard type as well
            extract_from_network_order(&reply_mac, data.sender_hard_addr.data());
            desc += mac_addr_to_str(reply_mac);

            break;

        }
        default: {
            desc += "<unsupported op>";
            break;
        }
    }

    ctx->res.push_back(make_unique<net_analyzer_parsed_prot>(
        net_analyzer_parsed_status::valid,
        net_analyzer_prot::arp,
        desc
    ));
}

void my_recv_ip4::handle_recv(pair<ip4_header, vector<uint8_t>> const& data) {
    // create ip4 desc
    string desc;

    desc += "src: ";
    desc += ip4_addr_to_str(data.first.src_addr);

    desc += ", ";
    desc += "dest: ";
    desc += ip4_addr_to_str(data.first.dest_addr);

    desc += ", ";
    desc += "prot: ";
    if (auto str = ip_prot_to_str(data.first.prot); str.has_value()) desc += str.value();
    else desc += "<unsupported>";

    // push parsed prot
    ctx->res.push_back(make_unique<net_analyzer_parsed_prot>(
        net_analyzer_parsed_status::valid,
        net_analyzer_prot::ip4,
        desc
    ));

    switch (data.first.prot) {
        case ip_prot_values::icmp: {
            struct tmp_net_access : public net_access {
                shared_ptr<recv_listener_bytes> the_recv;

            protected:
                shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}
                /* recv access */
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
            };
            shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();
            
            // arp_protocol::connect_recv(tmp_access, ctx->recv_arp, {});

            // tmp_access->the_recv->handle_recv(data.second);

            break;
        }
        case ip_prot_values::udp: {
            struct tmp_net_access : public net_access {
                shared_ptr<recv_listener_bytes> the_recv;

            protected:
                shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}
                /* recv access */
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
            };
            shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();
            
            // arp_protocol::connect_recv(tmp_access, ctx->recv_arp, {});

            // tmp_access->the_recv->handle_recv(data.second);

            break;
        }
        case ip_prot_values::tcp: {
            struct tmp_net_access : public net_access {
                shared_ptr<recv_listener_bytes> the_recv;

            protected:
                shared_ptr<send_medium_bytes> impl_get_send_access() override {return nullptr;}
                /* recv access */
                void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {the_recv = recv;}
            };
            shared_ptr<tmp_net_access> tmp_access = make_shared<tmp_net_access>();
            
            // arp_protocol::connect_recv(tmp_access, ctx->recv_arp, {});

            // tmp_access->the_recv->handle_recv(data.second);

            break;
        }
    }
}

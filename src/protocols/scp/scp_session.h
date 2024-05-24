//#ifndef SERVERCLIENT_SCP_SESSION_H
//#define SERVERCLIENT_SCP_SESSION_H
//
//#include "../../abstract/session/session_conn.h"
//#include "scp_common.h"
//
//class scp_session_conn : public session_conn {
//public:
//    scp_session_status status = CLOSED;
//
//    void init_session() {
//        send_msg<> msg;
//        uint8_t *buff = msg.get_active_buff();
//        *buff = scp_codes::START_SESSION_REQUEST;
//        msg.set_count(sizeof(scp_codes));
//        gateway->send_data(std::move(msg));
//        status = CREATING_INITIATOR;
//    }
//
//    // send
//    msg_sender* gateway = nullptr;
//    int send_data(send_msg<>&& msg) override {
//        switch (status) {
//            case CLOSED: return 0;
//            case OPEN: {
//                uint8_t *buff = msg.get_reserve_buff();
//                *buff = SESSION_DATA;
//                int cnt = msg.get_count();
//                memcpy(buff + 1, msg.get_active_buff(), cnt);
//
//                msg.toggle_active_buff();
//                msg.set_count(sizeof(scp_codes) + cnt);
//                return gateway->send_data(std::move(msg));
//            }
//            default: return 0;
//        }
//    }
//
//    // recv
//    void handle_received_event(received_msg&& event) override {
//        uint8_t * buff = event.data.data() + event.curr_offset;
//        scp_codes curr_code = static_cast<scp_codes>(*buff);
//
//        switch (status) {
//            case CLOSED: {
//                switch (curr_code) {
//                    case START_SESSION_REQUEST: {
//                        send_msg<> msg;
//                        uint8_t *buff = msg.get_active_buff();
//                        *buff = scp_codes::START_SESSION_CONFIRM;
//                        msg.set_count(sizeof(scp_codes));
//                        gateway->send_data(std::move(msg));
//                        status = OPEN; // jump directly to open status
//                        break;
//                    }
//                }
//                break;
//            }
//
//            case CREATING_INITIATOR: {
//                switch (curr_code) {
//                    case START_SESSION_CONFIRM: {
//                        status = OPEN;
//                        break;
//                    }
//                    case CLOSE_SESSION: {
//                        status = CLOSED;
//                        break;
//                    }
//                }
//                break;
//            }
//
//            case OPEN: {
//                switch (curr_code) {
//                    case CLOSE_SESSION: {
//                        status = CLOSED;
//                        break;
//                    }
//                    case SESSION_DATA: {
//                        connection::handle_received_event(std::move(event));
//                        break;
//                    }
//                }
//                break;
//            }
//        }
//
//    }
//
//};
//
//#endif //SERVERCLIENT_SCP_SESSION_H

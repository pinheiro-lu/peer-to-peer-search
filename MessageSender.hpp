#ifndef MESSAGESENDER_HPP
#define MESSAGESENDER_HPP

#include <string>

class MessageSender {
    private:
        int seqno = 1;
        int ttl = 100;
        std::string address;
        int port;
    public:
        MessageSender(std::string address, int port);
        bool send_hello(std::string neighbor_address, int neighbor_port);
        void choose_to_send_hello();
};

#endif
#ifndef MESSAGESENDER_HPP
#define MESSAGESENDER_HPP

#include <string>

#include "Message.hpp"

class MessageSender {
    private:
        int seqno = 1;
        int ttl = 100;
        std::string address;
        int port;
        int sockfd;
    public:
        bool send_message(std::string neighbor_address, int neighbor_port, Message &message);
        MessageSender(std::string address, int port);
        MessageSender();
        std::string get_address();
        int get_port();
        bool send_hello(std::string neighbor_address, int neighbor_port);
        bool send_search(std::string neighbor_address, int neighbor_port, std::string mode, std::string key, int hop_count);
        void send_reply(int sockfd, std::string response);
};

#endif
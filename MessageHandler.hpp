#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <string>

#include "NeighborManager.hpp"
#include "SearchManager.hpp"

class MessageHandler {
    public:
        void process_message(std::string message, std::string sender_address, NeighborManager &neighbor_manager, SearchManager &search_manager, int sockfd);
};

#endif
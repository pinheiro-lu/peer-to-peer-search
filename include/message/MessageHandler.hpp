#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <string>  // for string

#include "neighbor/NeighborManager.hpp"
#include "neighbor/SearchManager.hpp"

class SearchManager;
class SearchManager;
class SearchManager;

class MessageHandler {
    public:
        void process_message(std::string message, std::string sender_address, SearchManager &search_manager, int sockfd);
};

#endif
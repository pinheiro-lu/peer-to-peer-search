#include "MessageHandler.hpp"

#include "Message.hpp"

void MessageHandler::process_message(std::string message, std::string sender_address, NeighborManager &neighbor_manager, SearchManager &search_manager, int sockfd) {
    Message message_obj(message);

    if (message_obj.get_operation() == "HELLO") {
        neighbor_manager.process_hello_message(message_obj, sockfd);
    } else if (message_obj.get_operation() == "SEARCH") {
        if (message_obj.get_mode() == "FL") {
            search_manager.process_search_flooding_message(message_obj, sender_address);
        }
    }
}
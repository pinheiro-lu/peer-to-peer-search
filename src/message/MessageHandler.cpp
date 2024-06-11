#include "message/MessageHandler.hpp"

#include <iostream>                      // for operator<<, basic_ostream, endl

#include "message/Message.hpp"           // for Message
#include "keyvalue/KeyValueManager.hpp"  // for KeyValueManager
#include "message/MessageSender.hpp"     // for MessageSender
#include "neighbor/NeighborManager.hpp"  // for NeighborManager
#include "neighbor/SearchManager.hpp"    // for SearchManager

void MessageHandler::process_message(std::string message, std::string sender_address, SearchManager &search_manager, int sockfd) {
    Message message_obj(message);
    MessageSender response_sender;
    response_sender.send_reply(sockfd, message_obj.get_operation() + "_OK");

    if (message_obj.get_operation() == "HELLO") {
        search_manager.get_neighbor_manager().process_hello_message(message_obj);
    } else if (message_obj.get_operation() == "SEARCH") {
        if (message_obj.get_mode() == "FL") {
            search_manager.process_search_flooding_message(message_obj, sender_address);
        } else if (message_obj.get_mode() == "BP") {
            search_manager.process_search_depth_first_message(message_obj, sender_address);
        } else if (message_obj.get_mode() == "RW") {
            search_manager.process_search_random_walk_message(message_obj, sender_address);
        }
    } else if (message_obj.get_operation() == "VAL") {
        std::cout << "Valor encontrado! Chave: " << message_obj.get_key() << " valor: " << message_obj.get_value() << std::endl;
        search_manager.get_key_value_manager().add_key_value(message_obj.get_key(), message_obj.get_value(), message_obj.get_mode(), message_obj.get_hop_count());
    } else if (message_obj.get_operation() == "BYE") {
        search_manager.get_neighbor_manager().process_bye_message(message_obj);
    }

}
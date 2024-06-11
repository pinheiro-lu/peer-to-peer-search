#ifndef SEARCH_MANAGER_HPP
#define SEARCH_MANAGER_HPP

#include <string>
#include <unordered_set>

#include "MessageSender.hpp"
#include "KeyValueManager.hpp"
#include "NeighborManager.hpp"

class SearchManager {
    private:
        KeyValueManager key_value_manager;
        MessageSender message_sender;
        NeighborManager neighbor_manager;
        std::unordered_set<Message, MessageHash, MessageEqual> seen_messages;
    public:
        SearchManager(KeyValueManager &key_value_manager, MessageSender &message_sender, NeighborManager &neighbor_manager);
        void start_search_flooding(std::string key);
        void process_search_flooding_message(Message &message, std::string sender_address);
        void start_search_depth_first(std::string key);
        void process_search_depth_first_message(Message &message, std::string sender_address);
        void start_search_random_walk(std::string key);
        void process_search_random_walk_message(Message &message, std::string sender_address);
};

#endif
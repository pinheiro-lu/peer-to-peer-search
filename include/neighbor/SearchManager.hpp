#ifndef SEARCH_MANAGER_HPP
#define SEARCH_MANAGER_HPP

#include <string>                           // for string
#include <unordered_set>                    // for unordered_set
#include <unordered_map>                    // for unordered_map
#include <vector>                           // for vector
#include <mutex>                            // for mutex

#include "../message/MessageSender.hpp"     // for MessageSender
#include "../keyvalue/KeyValueManager.hpp"  // for KeyValueManager
#include "NeighborManager.hpp"              // for NeighborManager
#include "message/Message.hpp"              // for Message, MessageEqual (pt...
#include "neighbor/Neighbor.hpp"            // for Neighbor

class SearchManager {
    private:
        KeyValueManager& key_value_manager;
        MessageSender& message_sender;
        NeighborManager& neighbor_manager;
        std::unordered_set<Message, MessageHash, MessageEqual> seen_messages;
        std::unordered_map<Message, Neighbor, MessageHash, MessageEqual> parent_node;
        std::unordered_map<Message, Neighbor, MessageHash, MessageEqual> active_neighbor;
        std::unordered_map<Message, std::vector<Neighbor>, MessageHash, MessageEqual> candidate_neighbors;
        std::mutex data_mutex;
        int flooding_messages_seen = 0;
        int depth_first_messages_seen = 0;
        int random_walk_messages_seen = 0;
    public:
        SearchManager(KeyValueManager &key_value_manager, MessageSender &message_sender, NeighborManager &neighbor_manager);
        NeighborManager& get_neighbor_manager();
        KeyValueManager& get_key_value_manager();
        void start_search_flooding(std::string key);
        void process_search_flooding_message(Message &message, std::string sender_address);
        void start_search_depth_first(std::string key);
        void process_search_depth_first_message(Message &message, std::string sender_address);
        void start_search_random_walk(std::string key);
        void process_search_random_walk_message(Message &message, std::string sender_address);
        int get_flooding_messages_seen();
        int get_depth_first_messages_seen();
        int get_random_walk_messages_seen();
};

#endif
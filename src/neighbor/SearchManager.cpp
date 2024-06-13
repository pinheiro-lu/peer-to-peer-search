#include "neighbor/SearchManager.hpp"

#include <iostream>                      // for operator<<, endl, basic_ostream
#include <ctime>                         // for time
#include <cstdlib>                       // for rand, srand

#include "message/Message.hpp"           // for Message, MessageEqual, Messa...
#include "keyvalue/KeyValueManager.hpp"  // for KeyValueManager
#include "message/MessageSender.hpp"     // for MessageSender
#include "neighbor/NeighborManager.hpp"  // for NeighborManager

SearchManager::SearchManager(KeyValueManager &key_value_manager, MessageSender &message_sender, NeighborManager &neighbor_manager) : key_value_manager(key_value_manager), message_sender(message_sender), neighbor_manager(neighbor_manager) {
    std::srand(std::time(nullptr));
}

void SearchManager::start_search_flooding(std::string key) {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);

    if (key_value_manager.has_key(key)) {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
        return;
    }

    Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "FL", message_sender.get_port(), key);

    seen_messages.insert(message);

    for (Neighbor neighbor : neighbor_manager.get_neighbors()) {
        message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
    }
}

void SearchManager::process_search_flooding_message(Message &message, std::string sender_address) {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    // Increment number of seen messages
    flooding_messages_seen++;

    // Check if message has already been seen
    if (seen_messages.find(message) != seen_messages.end()) {
        std::cout << "\tFlooding: Mensagem repetida!" << std::endl;
        return;
    }

    seen_messages.insert(message);

    // Check if message is in the local key-value store and send a response for origin
    if (key_value_manager.has_key(message.get_key())) {
        std::cout << "\tChave encontrada!" << std::endl;
        Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "FL", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
        message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
        return;
    }

    // Decrement TTL
    message.decrement_ttl();

    // Update last hop port
    message.set_last_hop_port(message_sender.get_port());

    // Propagate message to neighbors if TTL > 0
    if (message.get_ttl() > 0) {
        for (auto &neighbor : neighbor_manager.get_neighbors()) {
            // Avoid sending message back to sender
            if (neighbor.get_address() == sender_address && neighbor.get_port() == message.get_last_hop_port()) {
                continue;
            }

            // Send message
            message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
        }
    } else {
        std::cout << "\tTTL igual a zero, descartando mensagem" << std::endl;
    }
}

NeighborManager& SearchManager::get_neighbor_manager() {
    return neighbor_manager;
}

KeyValueManager& SearchManager::get_key_value_manager() {
    return key_value_manager;
}

void SearchManager::start_search_depth_first(std::string key)
{
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    if (key_value_manager.has_key(key))
    {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
        return;
    }

    // Create message
    Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "BP", message_sender.get_port(), key);

    // Initialize parent node
    parent_node.emplace(message, Neighbor(message_sender.get_address(), message_sender.get_port()));

    // Initialize candidate neighbors
    candidate_neighbors[message] = neighbor_manager.get_neighbors();

    // Get a random neighbor to start the search
    if (!candidate_neighbors[message].empty())
    {
        int random_index = std::rand() % candidate_neighbors[message].size();
        active_neighbor.emplace(message, candidate_neighbors[message][random_index]);

        // Remove random neighbor from candidate neighbors
        candidate_neighbors[message].erase(candidate_neighbors[message].begin() + random_index);

        // Send message to random neighbor
        message_sender.send_message(active_neighbor.at(message).get_address(), active_neighbor.at(message).get_port(), message);
    }

}

void SearchManager::process_search_depth_first_message(Message &message, std::string sender_address)
{
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    // Increment number of seen messages
    depth_first_messages_seen++;

    // Check if message is in the local key-value store and send a response to origin
    if (key_value_manager.has_key(message.get_key()))
    {
        std::cout << "\tChave encontrada!" << std::endl;
        Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "BP", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
        message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
        return;
    }

    // Decrement TTL
    message.decrement_ttl();

    // Propagate message to one neighbor if TTL > 0
    if (message.get_ttl() > 0)
    {
        // Check if parent node is set
        if (parent_node.find(message) == parent_node.end())
        {
            // Set parent node and candidate neighbors
            parent_node.emplace(message, Neighbor(sender_address, message.get_last_hop_port()));
            candidate_neighbors[message] = neighbor_manager.get_neighbors();
        }

        // Remove sender from candidate neighbors
        for (auto it = candidate_neighbors[message].begin(); it != candidate_neighbors[message].end(); ++it)
        {
            if (it->get_address() == sender_address && it->get_port() == message.get_last_hop_port())
            {
                candidate_neighbors[message].erase(it);
                break;
            }
        }

        // Check if algorithm has finished
        if ((parent_node.at(message).get_address() == message_sender.get_address()) && (parent_node.at(message).get_port() == message_sender.get_port()) && (active_neighbor.at(message).get_address() == sender_address) && active_neighbor.at(message).get_port() == message.get_last_hop_port() && candidate_neighbors[message].empty())
        {
            std::cout << "\tBP: Nao foi possivel localizar a chave " << message.get_key() << std::endl;
            return;
        }

        Neighbor *next_neighbor;

        // Check if active neighbor is set and is not the sender
        if (active_neighbor.find(message) != active_neighbor.end() && (active_neighbor.at(message).get_address() != sender_address || active_neighbor.at(message).get_port() != message.get_last_hop_port()))
        {
            std::cout << "\tBP: Ciclo detectado, devolvendo a mensagem..." << std::endl;
            next_neighbor = new Neighbor(sender_address, message.get_last_hop_port());
        } else if (candidate_neighbors[message].empty()) {
            std::cout << "\tBP: nenhum vizinho encontrou a chave, retrocedendo..." << std::endl;
            next_neighbor = new Neighbor(parent_node.at(message));
        } else {
            int random_index = std::rand() % candidate_neighbors[message].size();
            next_neighbor = new Neighbor(candidate_neighbors[message][random_index]);
            active_neighbor.insert_or_assign(message, *next_neighbor);

            // Remove random neighbor from candidate neighbors
            candidate_neighbors[message].erase(candidate_neighbors[message].begin() + random_index);
        }

        // Update last hop port and send message
        message.set_last_hop_port(message_sender.get_port());
        message_sender.send_message(next_neighbor->get_address(), next_neighbor->get_port(), message);

        // Free memory
        delete next_neighbor;
        
    }
    else
    {
        std::cout << "\tTTL igual a zero, descartando mensagem" << std::endl;
    }
}

void SearchManager::start_search_random_walk(std::string key) {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    // Check if key is in the local key-value store
    if (key_value_manager.has_key(key)) {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
        return;
    }

    // Create message
    Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "RW", message_sender.get_port(), key);

    // Send message to a random neighbor
    const auto &neighbors = neighbor_manager.get_neighbors();
    if (!neighbors.empty()) {
        int random_index = std::rand() % neighbors.size();
        auto random_neighbor = neighbors[random_index];
        message_sender.send_message(random_neighbor.get_address(), random_neighbor.get_port(), message);
    }
}

void SearchManager::process_search_random_walk_message(Message &message, std::string sender_address) {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    // Increment number of seen messages
    random_walk_messages_seen++;

    // Check if message is in the local key-value store and send a response for origin
    if (key_value_manager.has_key(message.get_key())) {
        std::cout << "\tChave encontrada!" << std::endl;
        Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "RW", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
        message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
        return;
    }

    // Decrement TTL
    message.decrement_ttl();

    // Propagate message to a random neighbor if TTL > 0
    if (message.get_ttl() > 0) {
        const auto &neighbors = neighbor_manager.get_neighbors();
        if (!neighbors.empty()) {
            int random_index = std::rand() % neighbors.size();
            auto random_neighbor = neighbors[random_index];
            // Avoid sending message back to sender
            if (random_neighbor.get_address() == sender_address && random_neighbor.get_port() == message.get_last_hop_port()) {
                if (neighbors.size() == 1) {
                    std::cout << "\tA única opção de vizinho é o remetente, descartando mensagem" << std::endl;
                    return;
                }
                // select another neighbour if only one neighbour is the sender
                random_index = (random_index + 1) % neighbors.size();
                random_neighbor = neighbors[random_index];
            }

            // Update last hop port and send message
            message.set_last_hop_port(message_sender.get_port());
            message_sender.send_message(random_neighbor.get_address(), random_neighbor.get_port(), message);
        }
    } else {
        std::cout << "\tTTL igual a zero, descartando mensagem" << std::endl;
    }
}

int SearchManager::get_flooding_messages_seen() {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    return flooding_messages_seen;
}
int SearchManager::get_depth_first_messages_seen() {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    return depth_first_messages_seen;
}
int SearchManager::get_random_walk_messages_seen() {
    // Protect data against concurrent access
    std::lock_guard<std::mutex> lock(data_mutex);
    return random_walk_messages_seen;
}
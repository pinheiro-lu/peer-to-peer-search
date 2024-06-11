#include "SearchManager.hpp"

#include <iostream>
#include <stack>
#include <unordered_set>
#include <cstdlib>
#include <ctime>

#include "Message.hpp"

SearchManager::SearchManager(KeyValueManager &key_value_manager, MessageSender &message_sender, NeighborManager &neighbor_manager) : key_value_manager(key_value_manager), message_sender(message_sender), neighbor_manager(neighbor_manager) {
    std::srand(std::time(nullptr));
}


void SearchManager::start_search_flooding(std::string key) {
    if (key_value_manager.has_key(key)) {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
        return;
    }

    Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "FL", message_sender.get_port(), key);
    seen_messages.insert(message);

    for (auto &neighbor : neighbor_manager.get_neighbors()) {
        std::cout << "Enviando mensagem para " << neighbor.get_address() << ":" << neighbor.get_port() << std::endl;
        message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
    }
}

void SearchManager::process_search_flooding_message(Message &message, std::string sender_address) {
    // Check if message has already been seen
    if (seen_messages.find(message) != seen_messages.end()) {
        std::cout << "Flooding: Mensagem repetida!" << std::endl;
        return;
    }

    seen_messages.insert(message);

    // Check if message is in the local key-value store and send a response for origin
    if (key_value_manager.has_key(message.get_key())) {
        std::cout << "Chave encontrada!" << std::endl;
        Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "FL", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
        message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
    }

    // Decrement TTL
    message.decrement_ttl();

    // Propagate message to neighbors if TTL > 0
    if (message.get_ttl() > 0) {
        for (auto &neighbor : neighbor_manager.get_neighbors()) {
            // Avoid sending message back to sender
            if (neighbor.get_address() == sender_address && neighbor.get_port() == message.get_last_hop_port()) {
                continue;
            }

            // Update last hop port and send message
            message.set_last_hop_port(message_sender.get_port());
            message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
        }
    } else {
        std::cout << "TTL igual a zero, descartando mensagem" << std::endl;
    }
}

NeighborManager SearchManager::get_neighbor_manager() {
    return neighbor_manager;
}

    void SearchManager::start_search_depth_first(std::string key) {
        if (key_value_manager.has_key(key)) {
            std::cout << "Valor na tabela local!" << std::endl;
            std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
            return;
        }

        Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "DFS", message_sender.get_port(), key);
        seen_messages.insert(message);

        // Start DFS with a stack
        std::stack<Neighbor> dfs_stack;
        for (auto &neighbor : neighbor_manager.get_neighbors()) {
            dfs_stack.push(neighbor);
        }

        while (!dfs_stack.empty()) {
            Neighbor current = dfs_stack.top();
            dfs_stack.pop();

            message_sender.send_message(current.get_address(), current.get_port(), message);
        }
    }

    void SearchManager::process_search_depth_first_message(Message &message, std::string sender_address) {
        // Check if message has already been seen
        if (seen_messages.find(message) != seen_messages.end()) {
            std::cout << "DFS: Mensagem repetida!" << std::endl;
            return;
        }

        seen_messages.insert(message);

        // Check if message is in the local key-value store and send a response to origin
        if (key_value_manager.has_key(message.get_key())) {
            std::cout << "Chave encontrada!" << std::endl;
            Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "DFS", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
            message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
            return;
        }

        // Decrement TTL
        message.decrement_ttl();

        // Propagate message to one neighbor if TTL > 0
        if (message.get_ttl() > 0) {
            for (auto &neighbor : neighbor_manager.get_neighbors()) {
                // Avoid sending message back to sender
                if (neighbor.get_address() == sender_address && neighbor.get_port() == message.get_last_hop_port()) {
                    continue;
                }

                // Update last hop port and send message
                message.set_last_hop_port(message_sender.get_port());
                message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);

                // Only send to one neighbor
                break;
            }
        } else {
            std::cout << "TTL igual a zero, descartando mensagem" << std::endl;
        }
}

void SearchManager::start_search_random_walk(std::string key) {
    if (key_value_manager.has_key(key)) {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value_manager.get_value(key) << std::endl;
    }

    Message message = Message(message_sender.get_address(), message_sender.get_port(), "SEARCH", "RW", message_sender.get_port(), key);
    seen_messages.insert(message);

    // Choose a random neighbour to send message
    auto &neighbors = neighbor_manager.get_neighbors();
    if (!neighbors.empty()) {
        int random_index = std::rand() % neighbors.size();
        auto &random_neighbor = neighbors[random_index];
        message_sender.send_message(random_neighbor.get_address(), random_neighbor.get_port(), message);
    }
}

void SearchManager::process_search_random_walk_message(Message &message, std::string sender_address) {
    // Check if message has already been seen
    if (seen_messages.find(message) != seen_messages.end()) {
        std::cout << "Random Walk: Mensagem repetida!" << std::endl;
        return;
    }

    seen_messages.insert(message);

    // Check if message is in the local key-value store and send a response for origin
    if (key_value_manager.has_key(message.get_key())) {
        std::cout << "Chave encontrada!" << std::endl;
        Message response = Message(message_sender.get_address(), message_sender.get_port(), "VAL", "RW", message.get_key(), key_value_manager.get_value(message.get_key()), message.get_hop_count());
        message_sender.send_message(message.get_origin_address(), message.get_origin_port(), response);
    }

    // Decrement TTL
    message.decrement_ttl();

    // Propagate message to a random neighbor if TTL > 0
    if (message.get_ttl() > 0) {
        auto &neighbors = neighbor_manager.get_neighbors();
        if (!neighbors.empty()) {
            int random_index = std::rand() % neighbors.size();
            auto &random_neighbor = neighbors[random_index];
            // Avoid sending message back to sender
            if (random_neighbor.get_address() == sender_address && random_neighbor.get_port() == message.get_last_hop_port()) {
                if (neighbors.size() == 1) {
                    std::cout << "A única opção de vizinho é o remetente, descartando mensagem" << std::endl;
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
        std::cout << "TTL igual a zero, descartando mensagem" << std::endl;
    }
}
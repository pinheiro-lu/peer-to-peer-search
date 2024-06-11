#include "SearchManager.hpp"

#include <iostream>

#include "Message.hpp"

SearchManager::SearchManager(KeyValueManager &key_value_manager, MessageSender &message_sender, NeighborManager &neighbor_manager) : key_value_manager(key_value_manager), message_sender(message_sender), neighbor_manager(neighbor_manager) {}

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
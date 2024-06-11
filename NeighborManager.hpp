#ifndef NEIGHBOR_MANAGER_HPP
#define NEIGHBOR_MANAGER_HPP

#include <string>
#include <vector>
#include <fstream>

#include "Neighbor.hpp"
#include "SocketManager.hpp"
#include "MessageSender.hpp"

class NeighborManager {
    private:
        SocketManager socket_manager;
        std::vector<Neighbor> neighbors;
    public:
        NeighborManager(std::string address, int port);
        std::vector<Neighbor> get_neighbors();
        void add_neighbor(std::string address, int port);
        void list_neighbors();
        void add_neighbors_from_file(std::ifstream &neighbors_file, MessageSender &message_sender);
        void process_hello_message(Message &message);
        void process_bye_message(Message &message);
        SocketManager get_socket_manager();
};

#endif
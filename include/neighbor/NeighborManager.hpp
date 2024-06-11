#ifndef NEIGHBOR_MANAGER_HPP
#define NEIGHBOR_MANAGER_HPP

#include <string>                       // for string
#include <vector>                       // for vector
#include <fstream>                      // for ifstream

#include "Neighbor.hpp"                 // for Neighbor
#include "../socket/SocketManager.hpp"  // for SocketManager
#include "../message/MessageSender.hpp"

class Message;
class MessageSender;
class Message;
class MessageSender;
class Message;
class MessageSender;

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
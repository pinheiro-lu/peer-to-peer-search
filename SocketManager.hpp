#ifndef SOCKET_MANAGER_HPP
#define SOCKET_MANAGER_HPP

#include <string>
#include <vector>
#include <fstream>

#include "Neighbor.hpp"

class SocketManager {
    private:
        int sockfd;
        std::string address;
        int port;
        std::vector<Neighbor> neighbors;
        int seqno = 1;
        int ttl = 100;

        bool send_hello(std::string neighbor_address, int neighbor_port);
        void add_neighbor(std::string address, int port);

    public:
        SocketManager(std::string address, int port);
        ~SocketManager();
        void list_neighbors();
        void choose_to_send_hello();
        void add_neighbors_from_file(std::ifstream &neighbors_file);
        void listen_for_connections();
};

#endif
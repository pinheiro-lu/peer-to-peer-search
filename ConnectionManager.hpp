#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <string>

#include "NeighborManager.hpp"
#include "SearchManager.hpp"

class ConnectionManager {
    private:
        void handle_connection(int sockfd, struct sockaddr_in client_address, SearchManager &search_manager);
    public:
        void listen_for_connections(int sockfd, SearchManager &search_manager);
        int connect_to_neighbor(std::string neighbor_address, int neighbor_port);
};

#endif
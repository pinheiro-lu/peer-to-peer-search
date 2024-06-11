#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <string>

#include "NeighborManager.hpp"
#include "SearchManager.hpp"

class ConnectionManager {
    public:
        void listen_for_connections(int sockfd, NeighborManager &neighbor_manager, SearchManager &search_manager);
        int connect_to_neighbor(std::string neighbor_address, int neighbor_port);
};

#endif
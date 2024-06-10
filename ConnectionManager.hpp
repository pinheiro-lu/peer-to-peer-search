#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <string>

class ConnectionManager {
    public:
        std::string listen_for_connections(int sockfd);
        int connect_to_neighbor(std::string neighbor_address, int neighbor_port);
};

#endif
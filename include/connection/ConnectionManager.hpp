#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <string>         // for string
#include <mutex>          // for mutex
#include <unordered_set>  // for unordered_set

class SearchManager;

class ConnectionManager {
    private:
        std::unordered_set<int> connected_sockets;
        std::mutex socket_mutex; // Mutex to protect connected_sockets
        void handle_connection(int sockfd, struct sockaddr_in client_address, SearchManager &search_manager);
    public:
        void listen_for_connections(int sockfd, SearchManager &search_manager);
        int connect_to_neighbor(std::string neighbor_address, int neighbor_port);
        void close_all_connections();
};

#endif
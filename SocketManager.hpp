#ifndef SOCKET_MANAGER_HPP
#define SOCKET_MANAGER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cmath>

#include "Neighbor.hpp"

class SocketManager {
    private:
        int sockfd;
        std::string address;
        int port;
        std::vector<Neighbor> neighbors;
        int seqno = 1;
        int ttl = 100;
        std::map<std::string, int> search_message_counters;
        std::map<std::string, std::vector<int>> search_hop_counts;

        void increment_search_message_counter(const std::string& search_mode);
        void store_search_hop_count(const std::string& search_mode, int hop_count);        

        bool send_hello(std::string neighbor_address, int neighbor_port);
        void add_neighbor(std::string address, int port);

    public:
        SocketManager(std::string address, int port);
        ~SocketManager();
        void list_neighbors();
        void choose_to_send_hello();
        void add_neighbors_from_file(std::ifstream &neighbors_file);
        void listen_for_connections();
        void search_random_walk(const std::string& key, int seqno);
        void search_depth_first(const std::string& key, int seqno);
        void process_depth_first_search_message(const std::string& message);
        void process_val_message(const std::string& message);
        void display_statistics();
};

#endif
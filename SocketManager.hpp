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

    public:
        SocketManager(std::string address, int port);
        int get_sockfd();
        void close_socket();
};

#endif
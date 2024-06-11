#ifndef NEIGHBOR_HPP
#define NEIGHBOR_HPP

#include <string>

class Neighbor {
    private:
        std::string address;
        int port;
    public:
        Neighbor(std::string address, int port);
        std::string get_address();
        int get_port();
};
#endif
#ifndef NEIGHBOR_HPP
#define NEIGHBOR_HPP

#include <string>

class Neighbor {
    public:
        Neighbor(std::string address, int port) : address(address), port(port) {}
    private:
        std::string address;
        int port;
};
#endif
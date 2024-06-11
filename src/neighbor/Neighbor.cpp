#include "neighbor/Neighbor.hpp"

Neighbor::Neighbor(std::string address, int port) : address(address), port(port) {}

std::string Neighbor::get_address() {
    return address;
}

int Neighbor::get_port() {
    return port;
}
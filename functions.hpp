#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include <vector>

#include "Neighbor.hpp"

int create_socket (std::string address, int port);
void add_neighbors (std::ifstream &neighbors_file, std::vector<Neighbor> &neighbors);
void listen_for_connections (int sockfd);
void menu (std::vector<Neighbor> &neighbors, std::string address, int port);

#endif
#include <iostream>

#include "NeighborManager.hpp"

NeighborManager::NeighborManager(std::string address, int port) {
    socket_manager = SocketManager(address, port);
}

std::vector<Neighbor> NeighborManager::get_neighbors() {
    return neighbors;
}

void NeighborManager::add_neighbor(std::string address, int port) {
    // Check if the neighbor already exists
    for (Neighbor neighbor : neighbors) {
        if (neighbor.get_address() == address && neighbor.get_port() == port) {
            std::cout << "\tVizinho ja esta na tabela: " << address << ":" << port << std::endl;
            return;
        }
    }

    // Add neighbor
    std::cout << "\tAdicionando vizinho na tabela: " << address << ":" << port << std::endl;
    neighbors.push_back(Neighbor(address, port));
}

void NeighborManager::list_neighbors()
{
    // List neighbors
    std::cout << "Ha " << neighbors.size() << " vizinhos na tabela:" << std::endl;
    for (int i = 0; i < (int)neighbors.size(); i++)
    {
        std::cout << "\t" << "[" << i << "] " << neighbors[i].get_address() << " " << neighbors[i].get_port() << std::endl;
    }
}

void NeighborManager::add_neighbors_from_file(std::ifstream &neighbors_file)
{
    std::string line;
    while (std::getline(neighbors_file, line))
    {
        if (line.find(":") == std::string::npos)
        {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string neighbor_address = line.substr(0, line.find(":"));
        int neighbor_port = std::stoi(line.substr(line.find(":") + 1));
        if (neighbor_port <= 0)
        {
            std::cerr << "Porta inválida: " << neighbor_port << std::endl;
            continue;
        }

        // Add neighbor
        std::cout << "Tentando adicionar vizinho " << neighbor_address << ":" << neighbor_port << std::endl;

        if (send_hello(neighbor_address, neighbor_port))
            add_neighbor(neighbor_address, neighbor_port);
    }
}
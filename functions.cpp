#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>

#include "Neighbor.hpp"
#include "functions.hpp"

int create_socket (std::string address, int port) {
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return -1;
    }

    // Bind the socket to the address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // Convert port to network byte order
    switch (inet_pton(AF_INET, address.c_str(), &server_address.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return -1;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return -1;
    }
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Erro ao ligar socket ao endereço/porta" << std::endl;
        return -1;
    }

    return sockfd;
}

void add_neighbors (std::ifstream &neighbors_file, std::vector<Neighbor> &neighbors) {
    std::string line;
    while (std::getline(neighbors_file, line)) {
        if (line.find(":") == std::string::npos) {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string address = line.substr(0, line.find(":"));
        int port = std::stoi(line.substr(line.find(":") + 1));
        if (port <= 0) {
            std::cerr << "Porta inválida: " << port << std::endl;
            continue;
        }

        // Add neighbor
        std::cout << "Tentando adicionar vizinho " << address << ":" << port << std::endl;
        //TODO: send HELLO for neighbor and only add if it responds
        neighbors.push_back(Neighbor(address, port));
    }
}
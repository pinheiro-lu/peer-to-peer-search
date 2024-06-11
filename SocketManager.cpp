#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <thread>
#include <map>
#include <cmath>

#include "Neighbor.hpp"
#include "SocketManager.hpp"

SocketManager::SocketManager(std::string address, int port) : address(address), port(port){
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return;
    }

    // Bind the socket to the address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // Convert port to network byte order
    switch (inet_pton(AF_INET, address.c_str(), &server_address.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return;
    }
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Erro ao ligar socket ao endereço/porta" << std::endl;
        return;
    }
}

void SocketManager::close_socket() {
    close(sockfd);
}

int SocketManager::get_sockfd() {
    return sockfd;
}


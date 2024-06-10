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

SocketManager::~SocketManager() {
    close(sockfd);
}

void SocketManager::list_neighbors() {
    // List neighbors
    std::cout << "Ha " << neighbors.size() << " vizinhos na tabela:" << std::endl;
    for (int i = 0; i < (int) neighbors.size(); i++) {
        std::cout << "\t" << "[" << i << "] " << neighbors[i].get_address() << " " << neighbors[i].get_port() << std::endl;
    }
}

bool SocketManager::send_hello(std::string neighbor_address, int neighbor_port) {
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    // Connect to the neighbor
    struct sockaddr_in neighbor_sockaddr;
    neighbor_sockaddr.sin_family = AF_INET;
    neighbor_sockaddr.sin_port = htons(neighbor_port); // Convert port to network byte order
    switch (inet_pton(AF_INET, neighbor_address.c_str(), &neighbor_sockaddr.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return false;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return false;
    }
    if (connect(sockfd, (struct sockaddr *)&neighbor_sockaddr, sizeof(neighbor_sockaddr)) < 0) {
        std::cerr << "Erro ao conectar ao vizinho" << std::endl;
        return false;
    }

    // Increment sequence number
    seqno++;

    // Create HELLO message
    std::string message = address + ":" + std::to_string(port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " HELLO";

    // Send HELLO to neighbor
    if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return false;
    }

    // Buffer for received message
    char buffer[1024] = {0};

    // Receive message
    if (recv(sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cerr << "Erro ao receber mensagem" << std::endl;
        return false;
    }

    // Check if message is HELLO_OK
    std::string expected_message = "HELLO_OK";
    if (std::string(buffer).find(expected_message) == std::string::npos) {
        std::cerr << "Mensagem inesperada: " << buffer << std::endl;
        return false;
    }

    // Close socket
    close(sockfd);

    return true;
}

void SocketManager::choose_to_send_hello() {
    // Display neighbors
    std::cout << "Escolha o vizinho: " << std::endl;
    list_neighbors();
    int neighbor_index;
    std::cin >> neighbor_index;
    if (neighbor_index < 0 || neighbor_index >= (int) neighbors.size()) {
        std::cerr << "Vizinho inválido" << std::endl;
        return;
    }

    // Get neighbor
    Neighbor neighbor = neighbors[neighbor_index];

    send_hello(neighbor.get_address(), neighbor.get_port());
}

void SocketManager::add_neighbor(std::string neighbor_address, int neighbor_port) {
    // Check if neighbor is already in the list
    for (Neighbor neighbor : neighbors) {
        if (neighbor.get_address() == neighbor_address && neighbor.get_port() == neighbor_port) {
            std::cout << "\tVizinho ja esta na tabela: " << neighbor_address << ":" << neighbor_port << std::endl;
            return;
        }
    }

    // Add neighbor
    std::cout << "\tAdicionando vizinho na tabela: " << neighbor_address << ":" << neighbor_port << std::endl;
    neighbors.push_back(Neighbor(neighbor_address, neighbor_port));
}

void SocketManager::add_neighbors_from_file(std::ifstream &neighbors_file) {
    std::string line;
    while (std::getline(neighbors_file, line)) {
        if (line.find(":") == std::string::npos) {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string neighbor_address = line.substr(0, line.find(":"));
        int neighbor_port = std::stoi(line.substr(line.find(":") + 1));
        if (neighbor_port <= 0) {
            std::cerr << "Porta inválida: " << neighbor_port << std::endl;
            continue;
        }

        // Add neighbor
        std::cout << "Tentando adicionar vizinho " << neighbor_address << ":" << neighbor_port << std::endl;

        if (send_hello(neighbor_address, neighbor_port))
            add_neighbor(neighbor_address, neighbor_port);
    }
}

void SocketManager::listen_for_connections() {
    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) { // 5 is the maximum number of pending connections
        std::cerr << "Erro ao ouvir conexões" << std::endl;
        return;
    }

    // Accept incoming connections
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_size);
    if (client_sockfd < 0) {
        std::cerr << "Erro ao aceitar conexão" << std::endl;
        return;
    }

    // Buffer for received message
    char buffer[1024] = {0};

    // Receive message
    if (recv(client_sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cerr << "Erro ao receber mensagem" << std::endl;
        return;
    }

    // Parse message
    std::stringstream ss(buffer);
    std::string origin, message;
    int seqno, ttl;
    ss >> origin >> seqno >> ttl >> message;

    // Check if message is HELLO
    if (message.find("HELLO") == std::string::npos) {
        std::cerr << "Mensagem inesperada: " << message << std::endl;
        return;
    } 

    // Display received message
    std::cout << "Mensagem recebida: \"" << buffer << "\"" << std::endl;

    // Parse origin address and port
    std::string neighbor_address = origin.substr(0, origin.find(":"));
    int neighbor_port = std::stoi(origin.substr(origin.find(":") + 1));

    // Add origin to neighbors
    add_neighbor(neighbor_address, neighbor_port);

    // Create HELLO_OK message
    std::string response = "HELLO_OK";

    // Send HELLO_OK to origin
    if (send(client_sockfd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return;
    }
}

void SocketManager::add_key_values_from_file(std::ifstream &key_value_file) {
    std::string line;
    while (std::getline(key_value_file, line)) {
        if (line.find(" ") == std::string::npos) {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string key = line.substr(0, line.find(" "));
        std::string value = line.substr(line.find(" ") + 1);

        // Add key-value pair
        key_value[key] = value;
    }
}

void SocketManager::search_flooding() {
    std::cout << "Digite a chave a ser buscada" << std::endl;
    std::string key;
    std::cin >> key;

    // Check if key is in local table
    if (key_value.find(key) != key_value.end()) {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value[key] << std::endl;
        return;
    }

    // Create SEARCH message
    std::string message = address + ":" + std::to_string(port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " SEARCH FL " + std::to_string(port) + " " + key + " 1";

    // Send SEARCH to all neighbors
    for (Neighbor neighbor : neighbors) {
        // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "Erro ao criar socket" << std::endl;
            return;
        }

        // Connect to the neighbor
        struct sockaddr_in neighbor_sockaddr;
        neighbor_sockaddr.sin_family = AF_INET;
        neighbor_sockaddr.sin_port = htons(neighbor.get_port()); // Convert port to network byte order
        switch (inet_pton(AF_INET, neighbor.get_address().c_str(), &neighbor_sockaddr.sin_addr)) { // Convert address to network byte order
            case 0:
                std::cerr << "Endereço inválido" << std::endl;
                return;
            case -1:
                std::cerr << "Erro ao converter endereço" << std::endl;
                return;
        }
        if (connect(sockfd, (struct sockaddr *)&neighbor_sockaddr, sizeof(neighbor_sockaddr)) < 0) {
            std::cerr << "Erro ao conectar ao vizinho" << std::endl;
            return;
        }

        // Send SEARCH to neighbor
        if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Erro ao enviar mensagem" << std::endl;
            return;
        }

        // Close socket
        close(sockfd);
    }

}
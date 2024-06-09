#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <vector>

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

void add_neighbor (std::vector<Neighbor> &neighbors, std::string address, int port) {
    // Check if neighbor is already in the list
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

void listen_for_connections (int sockfd, std::vector<Neighbor> &neighbors) {
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
    std::string address = origin.substr(0, origin.find(":"));
    int port = std::stoi(origin.substr(origin.find(":") + 1));

    // Add origin to neighbors
    add_neighbor(neighbors, address, port);

    // Create HELLO_OK message
    std::string response = "HELLO_OK";

    // Send HELLO_OK to origin
    if (send(client_sockfd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return;
    }
}

void list_neighbors(std::vector<Neighbor> &neighbors) {
    // List neighbors
    std::cout << "Ha " << neighbors.size() << " vizinhos na tabela:" << std::endl;
    for (int i = 0; i < (int) neighbors.size(); i++) {
        std::cout << "\t" << "[" << i << "] " << neighbors[i].get_address() << " " << neighbors[i].get_port() << std::endl;
    }
}

void send_hello(std::vector<Neighbor> &neighbors, std::string address, int port, int &seqno, int &ttl) {
    // Display neighbors
    std::cout << "Escolha o vizinho: " << std::endl;
    list_neighbors(neighbors);
    int neighbor_index;
    std::cin >> neighbor_index;
    if (neighbor_index < 0 || neighbor_index >= (int) neighbors.size()) {
        std::cerr << "Vizinho inválido" << std::endl;
        return;
    }

    // Get neighbor
    Neighbor neighbor = neighbors[neighbor_index];

    // Increment sequence number
    seqno++;

    // Create HELLO message
    std::string message = address + ":" + std::to_string(port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " HELLO";

    // Send HELLO to neighbor
    std::cout << "Encaminhando mensagem \"" << message << "\" para " << neighbor.get_address() << ":" << neighbor.get_port() << std::endl;
    
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return;
    }

    // Connect to the neighbor
    struct sockaddr_in neighbor_address;
    neighbor_address.sin_family = AF_INET;
    neighbor_address.sin_port = htons(neighbor.get_port()); // Convert port to network byte order
    switch (inet_pton(AF_INET, neighbor.get_address().c_str(), &neighbor_address.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return;
    }
    if (connect(sockfd, (struct sockaddr *)&neighbor_address, sizeof(neighbor_address)) < 0) {
        std::cout << "Erro ao conectar ao vizinho" << std::endl; // on stdout because it's expected
        return;
    }

    // Send message
    if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return;
    }

    // Buffer for received message
    char buffer[1024] = {0};

    // Receive message
    if (recv(sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cout << "Erro ao receber mensagem" << std::endl;
        return;
    }

    // Check if message is HELLO_OK
    std::string expected_message = "HELLO_OK";
    if (std::string(buffer).find(expected_message) == std::string::npos) {
        std::cout << "Mensagem inesperada: " << buffer << std::endl;
        return;
    }

    std::cout << "\tEnvio feito com sucesso: \"" << message << "\"" << std::endl;

    // Close socket
    close(sockfd);
}

void menu(std::vector<Neighbor> &neighbors, std::string address, int port) {
    int ttl = 100, seqno = 1;
    // Display menu
    std::cout << "Escolha o comando:" << std::endl;
    std::cout << "\t[0] Listar vizinhos" << std::endl;
    std::cout << "\t[1] HELLO" << std::endl;
    std::cout << "\t[2] SEARCH (flooding)" << std::endl;
    std::cout << "\t[3] SEARCH (random walk)" << std::endl;
    std::cout << "\t[4] SEARCH (busca em profundidade)" << std::endl;
    std::cout << "\t[5] Estatisticas" << std::endl;
    std::cout << "\t[6] Alterar valor padrao de TTL" << std::endl;
    std::cout << "\t[9] Sair" << std::endl;

    // Wait for user input
    int command;
    std::cin >> command;
    switch (command) {
        case 0:
        list_neighbors(neighbors);
        break;
        case 1:
        send_hello(neighbors, address, port, seqno, ttl);
        break;
        case 2:
      //  search_flooding();
        break;
        case 3:
       // search_random_walk();
        break;
        case 4:
        //search_depth_first();
        break;
        case 5:
        //show_statistics();
        break;
        case 6:
        //change_ttl();
        break;
        case 9:
        //exit_program();
        break;
        default:
        std::cerr << "Comando inválido" << std::endl;
    }
}

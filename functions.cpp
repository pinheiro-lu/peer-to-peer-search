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

void listen_for_connections (int sockfd) {
    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        std::cerr << "Erro ao ouvir conexões" << std::endl;
        return;
    }
}

void menu() {
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
    //    list_neighbors();
        break;
        case 1:
     //   send_hello();
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
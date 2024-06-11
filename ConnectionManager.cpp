#include "ConnectionManager.hpp"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#include "MessageHandler.hpp"

void ConnectionManager::handle_connection(int client_sockfd, struct sockaddr_in client_address, NeighborManager &neighbor_manager, SearchManager &search_manager) {
    // Buffer for received message
    char buffer[1024] = {0};

    // Receive message
    if (recv(client_sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cerr << "Erro ao receber mensagem" << std::endl;
        return;
    }
    
    MessageHandler message_handler;

    // Pass the message
    message_handler.process_message(buffer, inet_ntoa(client_address.sin_addr), neighbor_manager, search_manager, client_sockfd);

    // Close the connection
    close(client_sockfd);
};

void ConnectionManager::listen_for_connections(int sockfd, NeighborManager &neighbor_manager, SearchManager &search_manager) {
    while (true)
    {
        // Listen for incoming connections
        if (listen(sockfd, 5) < 0)
        { // 5 is the maximum number of pending connections
            std::cerr << "Erro ao ouvir conexões" << std::endl;
            return;
        }

        // Accept incoming connections
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_sockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_size);
        if (client_sockfd < 0)
        {
            std::cerr << "Erro ao aceitar conexão" << std::endl;
            return;
        }
        // Create a thread to handle the connection
        std::thread connection_thread(&ConnectionManager::handle_connection, this, client_sockfd, client_address, std::ref(neighbor_manager), std::ref(search_manager));

        // Detach the thread
        connection_thread.detach();
    }
};

int ConnectionManager::connect_to_neighbor(std::string neighbor_address, int neighbor_port) {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return -1;
    }

    // Set up server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(neighbor_port);
    server_address.sin_addr.s_addr = inet_addr(neighbor_address.c_str());

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Erro ao conectar ao vizinho" << std::endl;
        return -1;
    }

    return sockfd;
};
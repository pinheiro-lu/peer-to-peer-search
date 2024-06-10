#include "MessageSender.hpp"
#include "ConnectionManager.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>


MessageSender::MessageSender(std::string address, int port) {
    this->address = address;
    this->port = port;
}

bool MessageSender::send_hello(std::string neighbor_address, int neighbor_port) {
    ConnectionManager connection_manager;
    int sockfd = connection_manager.connect_to_neighbor(neighbor_address, neighbor_port);
    if (sockfd == -1) {
        return false;
    } 
    // Increment sequence number
    seqno++;

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
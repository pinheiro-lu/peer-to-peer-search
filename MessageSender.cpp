#include "MessageSender.hpp"
#include "ConnectionManager.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>


MessageSender::MessageSender(std::string address, int port) {
    this->address = address;
    this->port = port;
}

MessageSender::MessageSender() {
};

std::string MessageSender::get_address() {
    return address;
}

int MessageSender::get_port() {
    return port;
}

bool MessageSender::send_message(std::string neighbor_address, int neighbor_port, Message &message) {
    // Show message
    std::cout << "Encaminhando mensagem: " << message.get_message() << " para " << neighbor_address << ":" << neighbor_port << std::endl;

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    // Connect to neighbor
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(neighbor_port);
    if (inet_pton(AF_INET, neighbor_address.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido" << std::endl;
        return false;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\tErro ao conectar" << std::endl;
        return false;
    }

    // Send message
    if (send(sockfd, message.get_message().c_str(), message.get_message().size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return false;
    }

    // Get response
    char buffer[1024] = {0};
    if (recv(sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cout << "\tErro ao receber mensagem" << std::endl;
        return false;
    }

    // Get operation from message
    std::istringstream iss(message.get_message());
    std::string operation;
    for (int i = 0; i < 4; i++) {
        iss >> operation;
    }

    // Check if response is <OPERATION>_OK
    std::string expected_message = operation + "_OK";
    if (std::string(buffer).find(expected_message) == std::string::npos) {
        std::cout << "\tMensagem inesperada: " << buffer << std::endl;
        return false;
    }

    // Print success message
    std::cout << "\tEnvio feito com sucesso: \"" << message.get_message() << "\"" << std::endl;

    // Close socket
    close(sockfd);

    return true;
}

void MessageSender::send_reply(int sockfd, std::string response) {
    // Send response
    if (send(sockfd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erro ao enviar resposta" << std::endl;
        return;
    }

    // Close socket
    close(sockfd);
}
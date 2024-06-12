#include "message/MessageSender.hpp"

#include <unistd.h>             // for close
#include <sys/socket.h>         // for send, AF_INET, connect, recv, setsockopt
#include <netinet/in.h>         // for sockaddr_in, htons
#include <arpa/inet.h>          // for inet_pton
#include <sys/time.h>           // for timeval
#include <iostream>             // for operator<<, endl, basic_ostream, ostream
#include <string>               // for allocator, string, char_traits, opera...
#include <sstream>              // for basic_istringstream, istringstream

#include "message/Message.hpp"  // for Message


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
    std::cout << "Encaminhando mensagem \"" << message.get_message() << "\" para " << neighbor_address << ":" << neighbor_port << std::endl;

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    // Bind socket to MessageSender address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido" << std::endl;
        return false;
    }
    addr.sin_port = htons(0);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Erro ao fazer bind" << std::endl;
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

    // Set timeout
    struct timeval tv;
    tv.tv_sec = 1; // 1 second timeout
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        std::cerr << "Erro ao definir timeout" << std::endl;
        return false;
    }

    // Get response
    char buffer[1024] = {0};
    int bytes_received = recv(sockfd, buffer, sizeof buffer - 1, 0); // -1 to leave space for null terminator
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "\tTimeout" << std::endl;
        } else {
            std::cout << "\tErro ao receber resposta" << std::endl;
        }
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
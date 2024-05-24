#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int create_socket (std::string address, int port) {
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return 1;
    }

    // Bind the socket to the address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // Convert port to network byte order
    switch (inet_pton(AF_INET, address.c_str(), &server_address.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return 1;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return 1;
    }
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Erro ao ligar socket ao endereço/porta" << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // Check if the user provided the address:port argument
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <endereco>:<porta> [vizinhos.txt [lista_chave_valor.txt]]" << std::endl;
        return 1;
    }

    // Parse address:port argument
    std::string address_port = argv[1];
    if (address_port.find(":") == std::string::npos) {
        std::cerr << "Argumento inválido" << std::endl;
        std::cerr << "Uso: " << argv[0] << " <endereco>:<porta> [vizinhos.txt [lista_chave_valor.txt]]" << std::endl;
        return 1;
    }

    // Parse address and port from address:port string
    std::string address = address_port.substr(0, address_port.find(":"));
    int port = std::stoi(address_port.substr(address_port.find(":") + 1));
    if (port <= 0) {
        std::cerr << "Porta inválida" << std::endl;
        return 1;
    }

    // Create a socket for the address and port
    if (int fail = create_socket(address, port)) {
        return fail;
    }


    return 0;
}
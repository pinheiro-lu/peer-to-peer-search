#include <fstream>
#include <iostream>
#include <vector>

#include "Neighbor.hpp"
#include "functions.hpp"

int main(int argc, char *argv[]) {
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

    // Create a vector to store neighbors
    std::vector<Neighbor> neighbors;

    // Add neighbors if provided
    if (argc > 2) {
        std::string neighbors_filename = argv[2];
        std::ifstream neighbors_file(neighbors_filename);
        if (!neighbors_file) {
            std::cerr << "Erro ao abrir arquivo de vizinhos" << std::endl;
            return 1;
        }

        add_neighbors(neighbors_file, neighbors);
    }


    return 0;
}
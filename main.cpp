#include <fstream>
#include <iostream>
#include <vector>
#include <thread>

#include "Neighbor.hpp"
#include "functions.hpp"
#include "SocketManager.hpp"
#include "NeighborManager.hpp"
#include "KeyValueManager.hpp"
#include "ConnectionManager.hpp"

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

    NeighborManager neighbor_manager = NeighborManager(address, port);
    KeyValueManager key_value_manager = KeyValueManager();
    MessageSender message_sender = MessageSender(address, port);

    // Add neighbors if provided
    if (argc > 2) {
        std::string neighbors_filename = argv[2];
        std::ifstream neighbors_file(neighbors_filename);
        if (!neighbors_file) {
            std::cerr << "Erro ao abrir arquivo de vizinhos" << std::endl;
            return 1;
        }

        neighbor_manager.add_neighbors_from_file(neighbors_file, message_sender);

        if (argc > 3) {
            std::string key_value_filename = argv[3];
            std::ifstream key_value_file(key_value_filename);
            if (!key_value_file) {
                std::cerr << "Erro ao abrir arquivo de chave-valor" << std::endl;
                return 1;
            }

            key_value_manager.add_key_values_from_file(key_value_file);
        }
    }

    ConnectionManager connection_manager = ConnectionManager();
    SearchManager search_manager = SearchManager(key_value_manager, message_sender, neighbor_manager);

    // Start to listen for incoming connections
    std::thread listener_thread([&]() { connection_manager.listen_for_connections(neighbor_manager.get_socket_manager().get_sockfd(), neighbor_manager, search_manager); });

    // Display menu and wait for user input
    while (true) {
        menu(neighbor_manager, message_sender, search_manager);
    }

    return 0;
}
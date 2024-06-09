#include <iostream>

#include "functions.hpp"
#include "SocketManager.hpp"

void menu(SocketManager &socket_manager) {
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
        socket_manager.list_neighbors();
        break;
        case 1:
        socket_manager.choose_to_send_hello();
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

#include <iostream>

#include "functions.hpp"
#include "SocketManager.hpp"
#include "NeighborManager.hpp"
#include "MessageSender.hpp"
#include "SearchManager.hpp" 
#include "Message.hpp"

void choose_to_send_hello(NeighborManager neighbor_manager, MessageSender message_sender)
{
  // Display neighbors
  std::cout << "Escolha o vizinho: " << std::endl;
  neighbor_manager.list_neighbors();
  int neighbor_index;
  std::cin >> neighbor_index;

  std::vector<Neighbor> neighbors = neighbor_manager.get_neighbors();

  if (neighbor_index < 0 || neighbor_index >= (int)neighbors.size())
  {
    std::cerr << "Vizinho inválido" << std::endl;
    return;
  }

  // Get neighbor
  Neighbor neighbor = neighbors[neighbor_index];
  Message message = Message(message_sender.get_address(), message_sender.get_port(), "HELLO");

  message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
}

void menu(NeighborManager &neighbor_manager, MessageSender &message_sender, SearchManager &search_manager) {
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
    std::string key;
    std::cin >> command;
    switch (command) {
        case 0:
        neighbor_manager.list_neighbors();
        break;
        case 1:
        choose_to_send_hello(neighbor_manager, message_sender);
        break;
        case 2:
        std::cout << "Digite a chave a ser buscada" << std::endl;
        std::cin >> key;
        search_manager.start_search_flooding(key);
        break;
        case 3:
        std::cout << "Digite a chave a ser buscada: ";
       // socket_manager.search_random_walk(key, 1); // 1 being the initial sequence number
        break;
        case 4:
        std::cout << "Digite a chave a ser buscada: ";
        //socket_manager.search_depth_first(key, 1);// 1 being the initial sequence number
        break;
        case 5:
        //show_statistics(socket_manager);
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

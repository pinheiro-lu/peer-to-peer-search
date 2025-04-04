#include <iostream>

#include "neighbor/NeighborManager.hpp"
#include "neighbor/SearchManager.hpp"
#include "connection/ConnectionManager.hpp"

void choose_to_send_hello(NeighborManager &neighbor_manager, MessageSender &message_sender)
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

float calc_average_hop_count(std::vector<int> &hop_count) {
  float sum = 0;
  for (int i = 0; i < (int)hop_count.size(); i++) {
    sum += hop_count[i];
  }
  return sum / hop_count.size();
}

void choose_to_send_bye(NeighborManager &neighbor_manager, MessageSender &message_sender) {
  // Send bye to all neighbors
  std::vector<Neighbor> neighbors = neighbor_manager.get_neighbors();
  for (Neighbor neighbor : neighbors) {
    Message message = Message(message_sender.get_address(), message_sender.get_port(), "BYE");
    message_sender.send_message(neighbor.get_address(), neighbor.get_port(), message);
  }

  // Close socket
  neighbor_manager.get_socket_manager().close_socket();
}

void show_statistics(SearchManager &search_manager) {
  std::cout << "Estatisticas" << std::endl;
  std::cout << "\tTotal de mensagens de flooding vistas: " << search_manager.get_flooding_messages_seen() << std::endl;
  std::cout << "\tTotal de mensagens de random walk vistas: " << search_manager.get_random_walk_messages_seen() << std::endl;
  std::cout << "\tTotal de mensagens de busca em profundidade vistas: " << search_manager.get_depth_first_messages_seen() << std::endl;
  std::cout << "\tMedia de saltos ate encontrar destino por flooding: " << calc_average_hop_count(search_manager.get_key_value_manager().get_flooding_hop_count()) << std::endl;
  std::cout << "\tMedia de saltos ate encontrar destino por random walk: " << calc_average_hop_count(search_manager.get_key_value_manager().get_random_walk_hop_count()) << std::endl;
  std::cout << "\tMedia de saltos ate encontrar destino por busca em profundidade: " << calc_average_hop_count(search_manager.get_key_value_manager().get_depth_first_hop_count()) << std::endl;
}

void menu(MessageSender &message_sender, SearchManager &search_manager, ConnectionManager &connection_manager) {
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
        search_manager.get_neighbor_manager().list_neighbors();
        break;
        case 1:
        choose_to_send_hello(search_manager.get_neighbor_manager(), message_sender);
        break;
        case 2:
        std::cout << "Digite a chave a ser buscada" << std::endl;
        std::cin >> key;
        search_manager.start_search_flooding(key);
        break;
        case 3:
        std::cout << "Digite a chave a ser buscada" << std::endl;
        std::cin >> key;
        search_manager.start_search_random_walk(key);
        break;
        case 4:
        std::cout << "Digite a chave a ser buscada" << std::endl;
        std::cin >> key;
        search_manager.start_search_depth_first(key);
        break;
        case 5:
        show_statistics(search_manager);
        break;
        case 6:
        std::cout << "Digite novo valor de TTL" << std::endl;
        std::cin >> command;
        Message::set_ttl(command);
        break;
        case 9:
        std::cout << "Voce escolheu 9" << std::endl;
        std::cout << "Saindo..." << std::endl;
        choose_to_send_bye(search_manager.get_neighbor_manager(), message_sender);
        connection_manager.close_all_connections();
        exit(0);
        default:
        std::cerr << "Comando inválido" << std::endl;
    }
}

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <thread>
#include <map>
#include <cmath>

#include "Neighbor.hpp"
#include "SocketManager.hpp"

SocketManager::SocketManager(std::string address, int port) : address(address), port(port){
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return;
    }

    // Bind the socket to the address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // Convert port to network byte order
    switch (inet_pton(AF_INET, address.c_str(), &server_address.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return;
    }
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Erro ao ligar socket ao endereço/porta" << std::endl;
        return;
    }
}

SocketManager::~SocketManager() {
    close(sockfd);
}

void SocketManager::list_neighbors() {
    // List neighbors
    std::cout << "Ha " << neighbors.size() << " vizinhos na tabela:" << std::endl;
    for (int i = 0; i < (int) neighbors.size(); i++) {
        std::cout << "\t" << "[" << i << "] " << neighbors[i].get_address() << " " << neighbors[i].get_port() << std::endl;
    }
}

bool SocketManager::send_hello(std::string neighbor_address, int neighbor_port) {
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    // Connect to the neighbor
    struct sockaddr_in neighbor_sockaddr;
    neighbor_sockaddr.sin_family = AF_INET;
    neighbor_sockaddr.sin_port = htons(neighbor_port); // Convert port to network byte order
    switch (inet_pton(AF_INET, neighbor_address.c_str(), &neighbor_sockaddr.sin_addr)) { // Convert address to network byte order
        case 0:
            std::cerr << "Endereço inválido" << std::endl;
            return false;
        case -1:
            std::cerr << "Erro ao converter endereço" << std::endl;
            return false;
    }
    if (connect(sockfd, (struct sockaddr *)&neighbor_sockaddr, sizeof(neighbor_sockaddr)) < 0) {
        std::cerr << "Erro ao conectar ao vizinho" << std::endl;
        return false;
    }

    // Increment sequence number
    seqno++;

    // Create HELLO message
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

void SocketManager::choose_to_send_hello() {
    // Display neighbors
    std::cout << "Escolha o vizinho: " << std::endl;
    list_neighbors();
    int neighbor_index;
    std::cin >> neighbor_index;
    if (neighbor_index < 0 || neighbor_index >= (int) neighbors.size()) {
        std::cerr << "Vizinho inválido" << std::endl;
        return;
    }

    // Get neighbor
    Neighbor neighbor = neighbors[neighbor_index];

    send_hello(neighbor.get_address(), neighbor.get_port());
}

void SocketManager::add_neighbor(std::string neighbor_address, int neighbor_port) {
    // Check if neighbor is already in the list
    for (Neighbor neighbor : neighbors) {
        if (neighbor.get_address() == neighbor_address && neighbor.get_port() == neighbor_port) {
            std::cout << "\tVizinho ja esta na tabela: " << neighbor_address << ":" << neighbor_port << std::endl;
            return;
        }
    }

    // Add neighbor
    std::cout << "\tAdicionando vizinho na tabela: " << neighbor_address << ":" << neighbor_port << std::endl;
    neighbors.push_back(Neighbor(neighbor_address, neighbor_port));
}

void SocketManager::add_neighbors_from_file(std::ifstream &neighbors_file) {
    std::string line;
    while (std::getline(neighbors_file, line)) {
        if (line.find(":") == std::string::npos) {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string neighbor_address = line.substr(0, line.find(":"));
        int neighbor_port = std::stoi(line.substr(line.find(":") + 1));
        if (neighbor_port <= 0) {
            std::cerr << "Porta inválida: " << neighbor_port << std::endl;
            continue;
        }

        // Add neighbor
        std::cout << "Tentando adicionar vizinho " << neighbor_address << ":" << neighbor_port << std::endl;

        if (send_hello(neighbor_address, neighbor_port))
            add_neighbor(neighbor_address, neighbor_port);
    }
}

void process_hello_message(int client_sockfd, const std::string &message) {
    // Display received message
    std::cout << "Mensagem recebida: \"" << message << "\"" << std::endl;

    // Create HELLO_OK message
    std::string response = "HELLO_OK";

    // Send HELLO_OK to origin
    if (send(client_sockfd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return;
    }
}

void SocketManager::listen_for_connections() {
    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) { // 5 is the maximum number of pending connections
        std::cerr << "Erro ao ouvir conexões" << std::endl;
        return;
    }

    // Accept incoming connections
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_size);
    if (client_sockfd < 0) {
        std::cerr << "Erro ao aceitar conexão" << std::endl;
        return;
    }

    // Buffer for received message
    char buffer[1024] = {0};

    // Receive message
    if (recv(client_sockfd, buffer, sizeof buffer, 0) < 0) {
        std::cerr << "Erro ao receber mensagem" << std::endl;
        return;
    }

    // Parse message
    std::stringstream ss(buffer);
    std::string origin, message;
    int seqno, ttl;
    ss >> origin >> seqno >> ttl >> message;

    if (message == "HELLO") {
        process_hello_message(client_sockfd, buffer);
    } else if (message == "SEARCH") {
        std::string search_mode;
        ss >> search_mode;
        if (search_mode == "RW") {
            process_random_walk_search_message(buffer);
        } else if (search_mode == "BP") {
            process_depth_first_search_message(buffer);
        } else if (search_mode == "FL") {
            process_flooding_search_message(client_sockfd, buffer);
        } else {
            std::cerr << "Modo de busca inválido: " << search_mode << std::endl;
        }
    } else if (message == "VAL") {
        process_val_message(buffer);
    } else {
        std::cerr << "Mensagem inesperada: " << buffer << std::endl;
    }

    // Check if message is HELLO
    if (message.find("HELLO") == std::string::npos) {
        std::cerr << "Mensagem inesperada: " << message << std::endl;
        return;
    } 

    // Display received message
    std::cout << "Mensagem recebida: \"" << buffer << "\"" << std::endl;

    // Parse origin address and port
    std::string neighbor_address = origin.substr(0, origin.find(":"));
    int neighbor_port = std::stoi(origin.substr(origin.find(":") + 1));

    // Add origin to neighbors
    add_neighbor(neighbor_address, neighbor_port);

    // Create HELLO_OK message
    std::string response = "HELLO_OK";

    // Send HELLO_OK to origin
    if (send(client_sockfd, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        return;
    }

        
}
void SocketManager::search_random_walk(const std::string &key, int seqno)
{
    // Choose a random neighbor
    std::string message = "<ORIGIN> " + std::to_string(seqno) + " " + std::to_string(ttl) + " SEARCH RW " + std::to_string(port) + " " + key + " 1";

    // Choose a random neighbor
    if (neighbors.empty())
    {
        std::cerr << "Nenhum vizinho disponível para enviar mensagem de busca." << std::endl;
        return;
    }
    int random_index = rand() % neighbors.size();
    Neighbor random_neighbor = neighbors[random_index];

    // Send message to the random neighbor
    if (!send_message_to_neighbor(message, random_neighbor))
    {
        std::cerr << "Erro ao enviar mensagem de busca para vizinho." << std::endl;
        return;
    }

    // Display sent search message
    std::cout << "Mensagem de busca enviada: " << message << std::endl;
}

bool SocketManager::send_message_to_neighbor(const std::string &message, const Neighbor &neighbor)
{
    // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    // Connect to the neighbor
    struct sockaddr_in neighbor_sockaddr;
    neighbor_sockaddr.sin_family = AF_INET;
    neighbor_sockaddr.sin_port = htons(neighbor.get_port()); // Convert port to network byte order
    switch (inet_pton(AF_INET, neighbor.get_address().c_str(), &neighbor_sockaddr.sin_addr))
    { // Convert address to network byte order
    case 0:
        std::cerr << "Endereço inválido" << std::endl;
        return false;
    case -1:
        std::cerr << "Erro ao converter endereço" << std::endl;
        return false;
    }
    if (connect(sockfd, (struct sockaddr *)&neighbor_sockaddr, sizeof(neighbor_sockaddr)) < 0)
    {
        std::cerr << "Erro ao conectar ao vizinho" << std::endl;
        return false;
    }

    // Send message to neighbor
    if (send(sockfd, message.c_str(), message.size(), 0) < 0)
    {
        std::cerr << "Erro ao enviar mensagem" << std::endl;
        close(sockfd);
        return false;
    }

    // Close socket
    close(sockfd);

    return true;
}

void SocketManager::search_depth_first(const std::string &key, int seqno)
{
    std::string origin = address + ":" + std::to_string(port);
    std::string message = "<ORIGIN> " + origin + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " SEARCH BP " + std::to_string(port) + " " + key + " 1";

    std::vector<Neighbor> candidate_neighbors(neighbors); // Copy neighbors to candidate neighbors
    Neighbor active_neighbor = choose_random_neighbor(candidate_neighbors);
    remove_neighbor_from_candidates(active_neighbor, candidate_neighbors);

    // Send message to active neighbor
    send_message_to_neighbor(message, active_neighbor);
}

Neighbor SocketManager::choose_random_neighbor(std::vector<Neighbor> &candidates)
{
    if (candidates.empty())
    {
        std::cerr << "No candidate neighbors available." << std::endl;
    }

    int random_index = rand() % candidates.size();
    return candidates[random_index];
}

void SocketManager::remove_neighbor_from_candidates(const Neighbor &neighbor, std::vector<Neighbor> &candidates)
{
    auto it = std::find_if(candidates.begin(), candidates.end(), [&](const Neighbor &n)
                           { return n.get_address() == neighbor.get_address() && n.get_port() == neighbor.get_port(); });

    if (it != candidates.end())
    {
        candidates.erase(it);
    }
}

void SocketManager::process_depth_first_search_message(const std::string &message)
{
    // Parse the message
    std::stringstream ss(message);
    std::string origin, last_hop_port, key;
    int seqno, ttl, hop_count;
    std::string search_mode;
    ss >> origin >> seqno >> ttl >> search_mode >> last_hop_port >> key >> hop_count;

    // Check if key is in local table
    if (key_is_in_local_table(key))
    {
        std::cout << "Chave encontrada! Enviando para origem da requisição." << std::endl;
        send_key_value_to_origin(key, origin);
        return;
    }

    // Decrement TTL
    ttl--;
    if (ttl <= 0)
    {
        std::cout << "TTL igual a zero, descartando mensagem." << std::endl;
        return;
    }

    // Check if it's the first time the message is seen
    if (hop_count == 1)
    {
        // Initialize variables if not initialized
        if (node_mother.empty())
        {
            node_mother = origin;
            candidate_neighbors = neighbors;
        }

        // Remove sender from candidate neighbors
        Neighbor sender(origin);
        remove_neighbor_from_candidates(sender, candidate_neighbors);
    }

    // Special conditions checks
    if (node_mother == address && active_neighbor == sender && candidate_neighbors.empty())
    {
        // End condition, couldn't find the key
        std::cout << "BP: Não foi possível localizar a chave " << key << std::endl;
        return;
    }

    if (!active_neighbor.empty() && active_neighbor != sender)
    {
        // Cycle detected, return the message
        std::cout << "BP: Ciclo detectado, devolvendo a mensagem..." << std::endl;
        send_message_to_neighbor(message, sender);
        return;
    }

    if (candidate_neighbors.empty())
    {
        // No neighbors found the key, backtrack
        std::cout << "BP: Nenhum vizinho encontrou a chave, retrocedendo..." << std::endl;
        send_message_to_neighbor(message, node_mother);
        return;
    }

    // Select a random neighbor to send the message
    active_neighbor = choose_random_neighbor(candidate_neighbors);
    remove_neighbor_from_candidates(active_neighbor, candidate_neighbors);

    // Increment hop count
    hop_count++;

    // Send message to active neighbor
    std::string new_message = "<ORIGIN> " + origin + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " " + search_mode + " " + last_hop_port + " " + key + " " + std::to_string(hop_count);
    send_message_to_neighbor(new_message, active_neighbor);
}

void SocketManager::add_key_values_from_file(std::ifstream &key_value_file)
{
    std::string line;
    while (std::getline(key_value_file, line))
    {
        if (line.find(" ") == std::string::npos)
        {
            std::cerr << "Linha inválida: " << line << std::endl;
            continue;
        }

        std::string key = line.substr(0, line.find(" "));
        std::string value = line.substr(line.find(" ") + 1);

        // Add key-value pair
        key_value[key] = value;
    }
}

void SocketManager::search_flooding()
{
    std::cout << "Digite a chave a ser buscada" << std::endl;
    std::string key;
    std::cin >> key;

    // Check if key is in local table
    if (key_value.find(key) != key_value.end())
    {
        std::cout << "Valor na tabela local!" << std::endl;
        std::cout << "\tchave: " << key << " valor: " << key_value[key] << std::endl;
        return;
    }

    // Create SEARCH message
    std::string message = address + ":" + std::to_string(port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " SEARCH FL " + std::to_string(port) + " " + key + " 1";

    // Send SEARCH to all neighbors
    for (Neighbor neighbor : neighbors)
    {
        // Create a socket for IPv4 (AF_INET), TCP (SOCK_STREAM), and default protocol (0)
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "Erro ao criar socket" << std::endl;
            return;
        }

        // Connect to the neighbor
        struct sockaddr_in neighbor_sockaddr;
        neighbor_sockaddr.sin_family = AF_INET;
        neighbor_sockaddr.sin_port = htons(neighbor.get_port()); // Convert port to network byte order
        switch (inet_pton(AF_INET, neighbor.get_address().c_str(), &neighbor_sockaddr.sin_addr)) { // Convert address to network byte order
            case 0:
                std::cerr << "Endereço inválido" << std::endl;
                return;
            case -1:
                std::cerr << "Erro ao converter endereço" << std::endl;
                return;
        }
        if (connect(sockfd, (struct sockaddr *)&neighbor_sockaddr, sizeof(neighbor_sockaddr)) < 0) {
            std::cerr << "Erro ao conectar ao vizinho" << std::endl;
            return;
        }

        // Send SEARCH to neighbor
        if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Erro ao enviar mensagem" << std::endl;
            return;
        }

        // Close socket
        close(sockfd);
    }
}
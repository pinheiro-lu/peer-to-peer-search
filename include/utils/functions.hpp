#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "../socket/SocketManager.hpp"
#include "../neighbor/NeighborManager.hpp"
#include "../message/MessageSender.hpp"
#include "../neighbor/SearchManager.hpp"
#include "../connection/ConnectionManager.hpp"

void menu(MessageSender &message_sender, SearchManager &search_manager, ConnectionManager &connection_manager);

#endif
#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "SocketManager.hpp"
#include "NeighborManager.hpp"
#include "MessageSender.hpp"
#include "SearchManager.hpp"
#include "ConnectionManager.hpp"

void menu(MessageSender &message_sender, SearchManager &search_manager, ConnectionManager &connection_manager);

#endif
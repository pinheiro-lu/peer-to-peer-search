# Peer-to-peer search system

Managing the socket connection (sockfd, address, port)
Managing neighbors (neighbors, add_neighbor, list_neighbors, add_neighbors_from_file)
Sending messages (send_hello, choose_to_send_hello)
Managing key-value pairs (key_value, add_key_values_from_file)
Managing message metadata (seqno, ttl)
Managing connections (listen_for_connections)
Managing search flooding (search_flooding)
Processing different types of messages (process_depth_first_search_message, process_val_message)
Displaying statistics (display_statistics)
You could consider splitting this class into smaller classes, each with its own responsibility. For example:

SocketManager: Manages the socket connection.
NeighborManager: Manages neighbors.
MessageSender: Sends messages.
KeyValueManager: Manages key-value pairs.
ConnectionManager: Manages connections and search flooding.
MessageProcessor: Processes different types of messages.
StatisticsDisplay: Displays statistics.
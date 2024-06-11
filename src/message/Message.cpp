#include "message/Message.hpp"

#include <sstream>

int Message::seqno = 0;
int Message::ttl = 100;

Message::Message(std::string message) {
    this->message = message;
    std::istringstream iss(message);
    std::string address_port;
    iss >> address_port >> message_seqno >> message_ttl >> operation;

    // Split address_port into address and port
    std::string delimiter = ":";
    size_t pos = 0;
    pos = address_port.find(delimiter);
    origin_address = address_port.substr(0, pos);
    origin_port = std::stoi(address_port.substr(pos + 1));

    if (operation == "SEARCH") {
        iss >> mode >> last_hop_port >> key >> hop_count;
    } else if (operation == "VAL") {
        iss >> mode >> key >> value >> hop_count;
    }

}

Message::Message(std::string origin_address, int origin_port, std::string operation) {
    this->origin_address = origin_address;
    this->origin_port = origin_port;
    message_seqno = ++seqno;
    this->message = origin_address + ":" + std::to_string(origin_port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " " + operation;
}

Message::Message(std::string origin_address, int origin_port, std::string operation, std::string mode, int last_hop_port, std::string key) {
    this->origin_address = origin_address;
    this->origin_port = origin_port;
    this->key = key;
    message_seqno = ++seqno;
    this->message = origin_address + ":" + std::to_string(origin_port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " " + operation + " " + mode + " " + std::to_string(last_hop_port) + " " + key + " " + std::to_string(hop_count);
    this->last_hop_port = last_hop_port;
}

Message::Message(std::string origin_address, int origin_port, std::string operation, std::string mode, std::string key, std::string value, int hop_count) {
    this->origin_address = origin_address;
    this->origin_port = origin_port;
    this->key = key;
    this->value = value;
    message_seqno = ++seqno;
    this->message = origin_address + ":" + std::to_string(origin_port) + " " + std::to_string(seqno) + " " + std::to_string(ttl) + " " + operation + " " + mode + " " + key + " " + value + " " + std::to_string(hop_count);
}

std::string Message::get_message() {
    return message;
}

std::string Message::get_key() {
    return key;
}  

std::string Message::get_value() {
    return value;
}

int Message::get_hop_count() {
    return hop_count;
}

std::string Message::get_origin_address() {
    return origin_address;
}

int Message::get_origin_port() {
    return origin_port;
}

std::string Message::get_message_identifier() const {
    return origin_address + " " + std::to_string(origin_port) + " " + std::to_string(message_seqno);
}

int Message::get_ttl() {
    return message_ttl;
}

int Message::get_last_hop_port() {
    return last_hop_port;
}

std::string Message::get_mode() {
    return mode;
}

std::string Message::get_operation() {
    return operation;
}

std::size_t MessageHash::operator() (const Message &message) const {
    return std::hash<std::string>()(message.get_message_identifier());
}

bool MessageEqual::operator() (const Message &lhs, const Message &rhs) const {
    return lhs.get_message_identifier() == rhs.get_message_identifier();
}

void Message::decrement_ttl() {
    --message_ttl;
}

void Message::set_last_hop_port(int last_hop_port) {
    this->last_hop_port = last_hop_port;
    ++hop_count;
}

void Message::set_ttl(int ttl) {
    Message::ttl = ttl;
}
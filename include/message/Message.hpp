#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <cstddef>

class Message {
    private:
        static int seqno;
        static int ttl;
        std::string origin_address;
        int origin_port;
        int message_seqno;
        int message_ttl = ttl;
        std::string operation;
        std::string mode;
        std::string key;
        std::string value;
        int hop_count = 1;
        int last_hop_port;
    public:
        static void set_ttl(int ttl);
        Message(std::string message);
        Message(std::string origin_address, int origin_port, std::string operation);
        Message(std::string origin_address, int origin_port, std::string operation, std::string mode, int last_hop_port, std::string key);
        Message(std::string origin_address, int origin_port, std::string operation, std::string mode, std::string key, std::string value, int hop_count);
        std::string get_message_identifier() const;
        std::string get_message();
        std::string get_key();
        std::string get_value();
        std::string get_origin_address();
        int get_origin_port();
        int get_hop_count();
        int get_ttl();
        std::string get_mode();
        std::string get_operation();
        void decrement_ttl();
        int get_last_hop_port();
        void set_last_hop_port(int last_hop_port);
};

struct MessageHash {
    std::size_t operator() (const Message &message) const;
};

struct MessageEqual {
    bool operator() (const Message &lhs, const Message &rhs) const;
};

#endif
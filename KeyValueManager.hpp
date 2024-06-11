#ifndef KEY_VALUE_MANAGER_HPP
#define KEY_VALUE_MANAGER_HPP

#include <map>
#include <string>
#include <vector>

class KeyValueManager {
    private:
        std::map<std::string, std::string> key_value;
        std::vector<int> flooding_hop_count;
        std::vector<int> depth_first_hop_count;
        std::vector<int> random_walk_hop_count;
    public:
        bool has_key(const std::string &key);
        std::string get_value(const std::string &key);
        void add_key_values_from_file(std::ifstream &key_value_file);
        void add_key_value(const std::string &key, const std::string &value, std::string mode, int hop_count);
        std::vector<int> &get_flooding_hop_count();
        std::vector<int> &get_depth_first_hop_count();
        std::vector<int> &get_random_walk_hop_count();
};

#endif

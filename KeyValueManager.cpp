#include "KeyValueManager.hpp"

#include <iostream>
#include <string>
#include <fstream> // Include the <fstream> header file


bool KeyValueManager::has_key(const std::string &key) {
    return key_value.find(key) != key_value.end();
}

std::string KeyValueManager::get_value(const std::string &key) {
    return key_value[key];
}

void KeyValueManager::add_key_values_from_file(std::ifstream &key_value_file) {
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

void KeyValueManager::add_key_value(const std::string &key, const std::string &value, std::string mode, int hop_count) {
    key_value[key] = value;
    if (mode == "FL") {
        flooding_hop_count.push_back(hop_count);
    } else if (mode == "BP") {
        depth_first_hop_count.push_back(hop_count);
    } else if (mode == "RW") {
        random_walk_hop_count.push_back(hop_count);
    }
}
std::vector<int> &KeyValueManager::get_flooding_hop_count() {
    return flooding_hop_count;
}

std::vector<int> &KeyValueManager::get_depth_first_hop_count() {
    return depth_first_hop_count;
}

std::vector<int> &KeyValueManager::get_random_walk_hop_count() {
    return random_walk_hop_count;
}

#ifndef KEY_VALUE_MANAGER_HPP
#define KEY_VALUE_MANAGER_HPP

#include <map>
#include <string>

class KeyValueManager {
    private:
        std::map<std::string, std::string> key_value;
    public:
        bool has_key(const std::string &key);
        std::string get_value(const std::string &key);
        void add_key_values_from_file(std::ifstream &key_value_file);
};

#endif

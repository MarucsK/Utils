#include <containers/map.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << std::boolalpha;
    Marcus::map<std::string, int> table;
    table["delay"] = 12;
    if (!table.contains("delay")) {
        table["delay"] = 32;
    }

    table["timeout"] = 42;

    for (auto it = table.begin(); it != table.end(); ++it) {
        std::cout << it->first << "=" << it->second << std::endl;
    }

    std::cout << "at(delay)" << table.at("delay") << std::endl;
    std::cout << "size: " << table.size() << std::endl;
}

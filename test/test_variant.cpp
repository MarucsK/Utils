#include <iostream>
#include <utility/variant.hpp>

void print(Marcus::variant<std::string, int, double> v) {
    v.visit([&](auto v) { std::cout << v << std::endl; });

    if (v.holds_alternative<std::string>()) {
        std::cout << v.get<std::string>() << std::endl;
    } else if (v.holds_alternative<int>()) {
        std::cout << v.get<int>() << std::endl;
    } else if (v.holds_alternative<double>()) {
        std::cout << v.get<double>() << std::endl;
    }
}

int main() {
    Marcus::variant<std::string, int, double> v1(Marcus::in_place_index<0>,
                                                 "Marcus");
    print(v1);
    Marcus::variant<std::string, int, double> v2(42);
    print(v2);
    Marcus::variant<std::string, int, double> v3 = 3.14;
    print(v3);
}

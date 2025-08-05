#include <containers/set.hpp>
#include <cstdio>
#include <iostream>

int main() {
    Marcus::multiset<int> table;
    table.insert(1);
    table.insert(2);
    table.insert(2);
    table.insert(2);
    table.insert(2);
    table.insert(-5);
    table.insert(3);
    table.emplace(-5);

    for (auto it = table.begin(); it != table.end(); ++it) {
        std::cout << *it << std::endl;
    }

    auto it = table.lower_bound(2);
    while (it != table.upper_bound(2)) {
        std::cout << *it++ << std::endl;
    }

    table.erase(2);
    for (const auto &s: table) {
        std::cout << s << std::endl;
    }

    Marcus::set<int> s;
    s.insert(1);
    s.insert(3);
    s.insert(5);
    s.emplace(4);
    printf("insert 4 = %d\n", s.insert(4).second); // 0
    printf("insert 6 = %d\n", s.insert(6).second); // 1
    printf("insert 7 = %d\n", s.insert(7).second); // 1
    printf("find 3 = %d\n", s.find(3) != s.end()); // 1
    printf("find 2 = %d\n", s.find(2) != s.end()); // 0
    printf("find 4 = %d\n", s.find(4) != s.end()); // 1

    for (auto it = s.begin(); it != s.end(); ++it) {
        std::cout << *it << std::endl;
    }

    s.erase(3);
    printf("find 3 = %d\n", s.find(3) != s.end()); // 0

    printf("min = %d\n", *s.begin());
    printf("max = %d\n", *s.rbegin());
    for (int i: s) {
        printf("%d\n", i);
    }
}

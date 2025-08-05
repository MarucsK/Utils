#include <containers/list.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>
#include <vector>

int main() {
    Marcus::list<int> arr{1, 2, 4, 5, 6};
    arr.erase(arr.cbegin(), std::next(arr.cbegin(), 2)); // 4 5 6
    arr.insert(arr.begin(), {40, 41, 42});               // 40 41 42 4 5 6
    for (int i = 0; i < 3; i++) { // 40 41 42 4 5 6 100 101 102
        arr.push_back(100 + i);   // O(1)
    }
    for (int i = 0; i < 3; i++) { // 202 201 200 40 41 42 4 5 6 100 101 102
        arr.push_front(200 + i);  // O(1)
    }
    size_t i = 0;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
        const int &val = *it;
        printf("arr[%zd] = %d\n", i, val);
        ++i;
    }
    Marcus::list<int> arr2 = arr;
    for (auto it = arr2.crbegin(); it != arr2.crend(); ++it) {
        const int &val = *it;
        --i;
        printf("arr[%zd] = %d\n", i, val);
    }
    printf("arr.size() = %zd\n", arr.size());
    arr2.erase(++arr2.begin(), arr2.end());
    std::cout << std::boolalpha << "arr2.size() = " << arr2.size()
              << ", arr2.empty() = " << arr2.empty() << '\n';
    arr2.erase(arr2.begin());
    std::cout << std::boolalpha << "arr2.size() = " << arr2.size()
              << ", arr2.empty() = " << arr2.empty() << '\n';
    Marcus::list<int> arr3(3);
    std::cout << arr3.size() << '\n';
}

#include <containers/vector.hpp>
#include <stdio.h>

int main() {
    Marcus::vector<int> arr;

    for (int i = 0; i < 16; i++) {
        printf("arr.push_back(%d)\n", i);
        arr.push_back(i);
    }

    arr.insert(arr.begin() + 3, {40, 41, 42});

    for (size_t i = 0; i < arr.size(); i++) {
        printf("arr[%zd] = %d\n", i, arr[i]);
    }

    Marcus::vector<int> bar(3);
    printf("arr.size() = %zd\n", arr.size());
    printf("bar.size() = %zd\n", bar.size());
    bar = std::move(arr);
    printf("arr.size() = %zd\n", arr.size());
    printf("bar.size() = %zd\n", bar.size());
    printf("sizeof(vector) = %zd\n", sizeof(Marcus::vector<int>));
}

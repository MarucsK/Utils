#include <containers/array.hpp>
#include <iostream>

template <typename T, size_t N>
void iota(Marcus::array<T, N> &a) noexcept {
    T count = 0;
    for (auto it = a.begin(), eit = a.end(); it != eit; ++it) {
        *it = count++;
    }
}

int main() {
    auto a = Marcus::array{2, 1, 0};
    for (auto &ai: a) {
        std::cout << ai << ' ';
    }
    std::cout << std::endl;

    iota(a);
    for (int i = 0; i <= 2; i++) {
        std::cout << a[i] << ' ';
    }

    std::cout << "front:" << a.front() << '\n';
    std::cout << "back:" << a.back() << '\n';

    for (auto it = a.rbegin(); it != a.rend(); ++it) {
        std::cout << *it << ' ';
    }
}

#include <cstdio>
#include <iostream>
#include <utility/functional.hpp>

void func_hello(int i) {
    printf("#%d Hello\n", i);
}

struct func_printnum_t {
    void operator()(int i) const {
        printf("#%d Numbers are: %d, %d\n", i, x, y);
    }

    int x;
    int y;
};

void repeattwice(const Marcus::Function<void(int)> &func) {
    func(1);
    func(2);
}

int main() {
    int x = 4;
    int y = 2;
    repeattwice([=](int i) { printf("#%d Numbers are: %d, %d\n", i, x, y); });

    func_printnum_t func_printnum{x, y};
    repeattwice(func_printnum);
    repeattwice(func_hello);

    Marcus::Function<void(int)> f{[](int i) {
        printf("i = %d\n", i);
    }};

    f(2);
    auto ff = f;
    ff(3);

    return 0;
}

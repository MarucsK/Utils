#include <cstdio>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility/optional.hpp>
#include <vector>

struct C {
    C(int x, int y) : m_x(x), m_y(y) {
        printf("C(x,y) \n");
    }

    C(const C &) {
        printf("C(C const &)\n");
    }

    C &operator=(const C &) {
        printf("C &operator=(C const &)\n");
        return *this;
    }

    C &operator=(C &&) {
        printf("C &operator=(C &&)\n");
        return *this;
    }

    ~C() {
        printf("~C()\n");
    }

    int value() const {
        return m_x;
    }

    int m_x;
    int m_y;
};

namespace CC {
struct C {
    C(int x, int y) : m_x(x), m_y(y) {
        printf("C(x,y) \n");
    }

    C(const C &) {
        printf("C(C const &)\n");
    }

    C(C &&) {
        printf("C(C &&)\n");
    }

    C &operator=(const C &) {
        printf("C &operator=(C const &)\n");
        return *this;
    }

    C &operator=(C &&) {
        printf("C &operator=(C &&)\n");
        return *this;
    }

    ~C() {
        printf("~C()\n");
    }

    int value() const {
        return m_x;
    }

    int m_x;
    int m_y;
};

void swap(C &c1, C &c2) {
    (void)c1, (void)c2;
    printf("swap(C &, C &)\n");
}
} // namespace CC

void test_self() {
    Marcus::optional<C> optc(Marcus::nullopt);

    Marcus::optional<std::string> opts("ss");

    opts.value();            // std::string &
    std::move(opts).value(); // std::string &&

    auto s1 = std::move(opts.value());
    auto s2 = std::move(opts.value());

    const Marcus::optional<std::string> optcs("ss");
    optcs.value();

    Marcus::optional<int> opt(1);
    std::cout << opt.has_value() << std::endl; // true
    std::cout << opt.value() << std::endl;     // 1
    std::cout << opt.value_or(0) << std::endl; // 1

    opt = Marcus::nullopt;
    std::cout << opt.has_value() << std::endl; // false

    opt = 42;
    std::cout << opt.has_value() << '\n'; // true
    std::cout << opt.value() << '\n';     // 42
    std::cout << opt.value_or(0) << '\n'; // 42

    opt = Marcus::optional<int>(42);
    std::cout << opt.has_value() << '\n'; // true
    std::cout << opt.value() << '\n';     // 42
    std::cout << opt.value_or(0) << '\n'; // 42

    opt = Marcus::optional<int>(Marcus::nullopt);
    std::cout << opt.has_value() << '\n'; // false

    Marcus::optional<int> opt2(Marcus::nullopt);
    std::cout << opt2.has_value() << '\n'; // false
    try {
        opt2.value();
    } catch (const Marcus::BadOptionalAccess &e) {
        std::cout << "opt2 exception: " << e.what() << '\n';
    }
    std::cout << opt2.value_or(0) << '\n'; // 0

    Marcus::optional<int> opt3 = opt2;
    std::cout << opt3.has_value() << '\n'; // false
    try {
        opt2.value();
    } catch (const Marcus::BadOptionalAccess &e) {
        std::cout << "opt3 exception: " << e.what() << '\n';
    }
    std::cout << opt3.value_or(0) << '\n'; // 0
}

void test_std() {
    std::optional<int> opt(1);
    std::cout << opt.has_value() << '\n'; // true
    std::cout << opt.value() << '\n';     // 1
    std::cout << opt.value_or(0) << '\n'; // 1

    std::optional<int> opt2(std::nullopt);
    std::cout << opt2.has_value() << '\n'; // false
    try {
        opt2.value();
    } catch (const std::bad_optional_access &) {
        std::cout << "opt2 exception ok\n";
    }
    std::cout << opt2.value_or(0) << '\n'; // 0

    opt2 = 42;
    std::cout << opt2.has_value() << '\n'; // true
    std::cout << opt2.value() << '\n';     // 42
    std::cout << opt2.value_or(0) << '\n'; // 42
}

Marcus::optional<int> parseInt(std::string s) {
    try {
        return std::stoi(s);
    } catch (const std::invalid_argument &) {
        return Marcus::nullopt;
    }
}

Marcus::optional<int> getInt(std::istream &is) {
    std::string s;
    is >> s;
    if (!is.good()) {
        return Marcus::nullopt;
    }
    return parseInt(s);
}

void test_emplace() {
    while (auto opt = getInt(std::cin)) {
        std::cout << *opt << '\n';
    }

    Marcus::optional<C> opt;

    Marcus::optional<C> optc;
    optc = C(1, 2);     // C(x,y) C(C const &) ~C() ~C()
    optc.emplace(1, 2); // C(x,y)
    optc.value();
    std::cout << (*optc).m_x << '\n';
    std::cout << optc->m_x << '\n';

    Marcus::optional<int> opti;
    opti.emplace(42);
    opti.emplace();

    int i = static_cast<bool>(opti);
    std::cout << i << '\n';

    Marcus::optional<std::string> opts = Marcus::nullopt;
    opts.emplace();

    opts.reset();
}

void test_cmp() {
    Marcus::optional<int> o = Marcus::nullopt;
    std::cout << (o != Marcus::optional(100)) << '\n';

    auto x = Marcus::make_optional(42);

    o = -42;
    auto o2 = o.and_then([&](int i) -> Marcus::optional<int> {
        if (i < 0) {
            return Marcus::nullopt;
        }
        return i + 1;
    });

    if (o2) {
        std::cout << *o2 << '\n';
    } else {
        std::cout << "nullopt\n";
    }

    o = -42;
    std::unique_ptr<int> up = std::make_unique<int>();
    auto o3 = o.transform([up = std::move(up)](int i) -> int { return i + 1; });
    if (o3) {
        std::cout << o3.value() << '\n';
    } else {
        std::cout << "nullopt\n";
    }

    o = 42;
    auto o4 = o.or_else([]() -> Marcus::optional<int> {
        std::cout << "no value! \n";
        return 0;
    });
    if (o4) {
        std::cout << o4.value() << '\n';
    } else {
        std::cout << "nullopt\n";
    }
}

void test_in_place() {
    Marcus::optional<C> o1 = C(1, 2);              // C(x,y) C(C const &)
    o1.emplace(1, 2);                              // C(x,y)

    Marcus::optional<C> o2(Marcus::inPlace, 1, 2); // C(x,y)
    Marcus::optional<C> o3(C(1, 2));               // C(x,y) C(C const &)
    Marcus::optional<std::vector<int>> ov(Marcus::inPlace, {1, 2, 3});

    Marcus::optional<CC::C> ccp(Marcus::inPlace, 1, 2); // C(x,y)
    Marcus::optional<CC::C> uur(Marcus::inPlace, 3, 4); // C(x,y)
    ccp.swap(uur);
}

int main() {
    std::cout << std::boolalpha;
    test_std();
    std::cout << "---------------" << std::endl;
    test_self();
    std::cout << "---------------" << std::endl;
    test_emplace();
    std::cout << "---------------" << std::endl;
    test_cmp();
    std::cout << "---------------" << std::endl;
    test_in_place();
}

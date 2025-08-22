#include <algorithm>
#include <cassert>
#include <containers/deque.hpp>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T, typename Alloc>
void print_deque(const Marcus::deque<T, Alloc> &d, const std::string &name) {
    std::cout << name << " (size: " << d.size()
              << ", empty: " << (d.empty() ? "true" : "false") << "): [";
    bool first = true;
    for (const auto &val: d) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << val;
        first = false;
    }
    std::cout << "]" << std::endl;
}

struct MyClass {
    int id;
    static int s_live_count;
    static int s_constructed;
    static int s_destroyed;

    MyClass(int i = 0) : id(i) {
        s_live_count++;
        s_constructed++;
    }

    MyClass(const MyClass &other) : id(other.id) {
        s_live_count++;
        s_constructed++;
    }

    MyClass(MyClass &&other) noexcept : id(other.id) {
        s_live_count++;
        s_constructed++;
        other.id = -1;
    }

    MyClass &operator=(const MyClass &other) {
        if (this != &other) {
            id = other.id;
        }
        return *this;
    }

    MyClass &operator=(MyClass &&other) noexcept {
        if (this != &other) {
            id = other.id;
            other.id = -1;
        }
        return *this;
    }

    ~MyClass() {
        s_live_count--;
        s_destroyed++;
    }

    bool operator==(const MyClass &other) const {
        return id == other.id;
    }

    bool operator<(const MyClass &other) const {
        return id < other.id;
    }
#if __cpp_lib_three_way_comparison
    std::strong_ordering operator<=>(const MyClass &other) const = default;
#endif

    friend std::ostream &operator<<(std::ostream &os, const MyClass &mc) {
        return os << "MC(" << mc.id << ")";
    }

    static void reset_counts() {
        s_live_count = 0;
        s_constructed = 0;
        s_destroyed = 0;
    }
};

int MyClass::s_live_count = 0;
int MyClass::s_constructed = 0;
int MyClass::s_destroyed = 0;

void test_constructors() {
    std::cout << "\n--- Testing Constructors ---\n";

    Marcus::deque<int> d1;
    print_deque(d1, "d1 (default)");
    assert(d1.empty());
    assert(d1.size() == 0);

    Marcus::deque<int> d2(5);
    print_deque(d2, "d2 (size 5, default)");
    assert(d2.size() == 5);
    for (int i = 0; i < 5; ++i) {
        assert(d2[i] == 0);
    }

    Marcus::deque<int> d3(3, 10);
    print_deque(d3, "d3 (size 3, value 10)");
    assert(d3.size() == 3);
    for (int i = 0; i < 3; ++i) {
        assert(d3[i] == 10);
    }

    std::vector<int> v = {1, 2, 3, 4, 5};
    Marcus::deque<int> d4(v.begin(), v.end());
    print_deque(d4, "d4 (range from vector)");
    assert(d4.size() == 5);
    for (int i = 0; i < 5; ++i) {
        assert(d4[i] == v[i]);
    }

    Marcus::deque<int> d5 = {100, 200, 300};
    print_deque(d5, "d5 (initializer list)");
    assert(d5.size() == 3);
    assert(d5[0] == 100 && d5[1] == 200 && d5[2] == 300);

    Marcus::deque<int> d6 = d4;
    print_deque(d6, "d6 (copy of d4)");
    assert(d6.size() == d4.size());
    assert(std::equal(d6.begin(), d6.end(), d4.begin()));

    Marcus::deque<int> d7 = std::move(d5);
    print_deque(d7, "d7 (move of d5)");
    print_deque(d5, "d5 (after move)");
    assert(d7.size() == 3);
    assert(d7[0] == 100 && d7[1] == 200 && d7[2] == 300);
    assert(d5.empty());

    std::cout << "Constructors tests passed.\n";
}

void test_element_access() {
    std::cout << "\n--- Testing Element Access ---\n";
    Marcus::deque<int> d = {10, 20, 30, 40, 50};
    print_deque(d, "d");

    assert(d.front() == 10);
    assert(d.back() == 50);
    assert(d[2] == 30);
    assert(d.at(4) == 50);

    d.front() = 100;
    d.back() = 500;
    d[1] = 200;
    assert(d.front() == 100);
    assert(d.back() == 500);
    assert(d[1] == 200);
    print_deque(d, "d (modified)");

    bool caught_exception = false;
    try {
        d.at(5);
    } catch (const std::out_of_range &e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
        caught_exception = true;
    }
    assert(caught_exception);

    std::cout << "Element access tests passed.\n";
}

void test_iterators() {
    std::cout << "\n--- Testing Iterators ---\n";
    Marcus::deque<int> d = {1, 2, 3, 4, 5};
    print_deque(d, "d");

    int i = 1;
    for (auto it = d.begin(); it != d.end(); ++it) {
        assert(*it == i);
        i++;
    }
    assert(i == 6);

    i = 5;
    for (auto it = d.rbegin(); it != d.rend(); ++it) {
        assert(*it == i);
        i--;
    }
    assert(i == 0);

    const Marcus::deque<int> &cd = d;
    i = 1;
    for (auto it = cd.cbegin(); it != cd.cend(); ++it) {
        assert(*it == i);
        i++;
    }

    auto it = d.begin();
    assert(*(it + 2) == 3);
    assert(*(it + 4) == 5);
    it += 3;
    assert(*it == 4);
    it -= 2;
    assert(*it == 2);

    auto it_end = d.end();
    assert(it_end - d.begin() == 5);

    std::cout << "Iterators tests passed.\n";
}

void test_capacity() {
    std::cout << "\n--- Testing Capacity ---\n";
    Marcus::deque<int> d;
    assert(d.empty());
    assert(d.size() == 0);

    d.push_back(1);
    assert(!d.empty());
    assert(d.size() == 1);

    d.resize(5);
    print_deque(d, "d (resized to 5)");
    assert(d.size() == 5);
    assert(d[0] == 1);
    assert(d[1] == 0);

    d.resize(2, 99);
    print_deque(d, "d (resized to 2, with value 99)");
    assert(d.size() == 2);
    assert(d[0] == 1);
    assert(d[1] == 0);

    d.resize(4, 77);
    print_deque(d, "d (resized to 4, with value 77)");
    assert(d.size() == 4);
    assert(d[0] == 1);
    assert(d[1] == 0);
    assert(d[2] == 77);
    assert(d[3] == 77);

    Marcus::deque<int> d_s2f;
    for (int i = 0; i < 100; ++i) {
        d_s2f.push_back(i);
    }
    std::cout << "Before shrink_to_fit: d_s2f size: " << d_s2f.size()
              << std::endl;
    d_s2f.shrink_to_fit();
    std::cout << "After shrink_to_fit: d_s2f size: " << d_s2f.size()
              << std::endl;
    assert(d_s2f.size() == 100);
    for (int i = 0; i < 100; ++i) {
        assert(d_s2f[i] == i);
    }

    Marcus::deque<int> empty_d;
    empty_d.shrink_to_fit();
    assert(empty_d.empty());
    assert(empty_d.size() == 0);

    std::cout << "Capacity tests passed.\n";
}

void test_push_pop() {
    std::cout << "\n--- Testing Push/Pop ---\n";
    Marcus::deque<int> d;

    d.push_back(10);
    print_deque(d, "d (push_back 10)");
    assert(d.size() == 1 && d.front() == 10);

    d.push_front(5);
    print_deque(d, "d (push_front 5)");
    assert(d.size() == 2 && d.front() == 5 && d.back() == 10);

    d.emplace_back(20);
    print_deque(d, "d (emplace_back 20)");
    assert(d.size() == 3 && d.back() == 20);

    d.emplace_front(1);
    print_deque(d, "d (emplace_front 1)");
    assert(d.size() == 4 && d.front() == 1);

    d.pop_back();
    print_deque(d, "d (pop_back)");
    assert(d.size() == 3 && d.back() == 10);

    d.pop_front();
    print_deque(d, "d (pop_front)");
    assert(d.size() == 2 && d.front() == 5);

    d.pop_back();
    d.pop_front();
    print_deque(d, "d (after all pops)");
    assert(d.empty());

    std::cout << "\n--- Testing Push/Pop with MyClass ---\n";
    MyClass::reset_counts();
    Marcus::deque<MyClass> mc_deque;
    mc_deque.push_back(MyClass(1));
    mc_deque.emplace_front(2);
    mc_deque.push_back(MyClass(3));
    mc_deque.emplace_front(4);
    print_deque(mc_deque, "mc_deque");
    assert(MyClass::s_live_count == 4);

    mc_deque.pop_back();
    print_deque(mc_deque, "mc_deque (pop_back)");
    assert(MyClass::s_live_count == 3);

    mc_deque.pop_front();
    print_deque(mc_deque, "mc_deque (pop_front)");
    assert(MyClass::s_live_count == 2);

    mc_deque.clear();
    print_deque(mc_deque, "mc_deque (clear)");
    assert(MyClass::s_live_count == 0);
    assert(MyClass::s_constructed == MyClass::s_destroyed);

    std::cout << "Push/Pop tests passed.\n";
}

void test_insert() {
    std::cout << "\n--- Testing Insert ---\n";
    Marcus::deque<int> d = {1, 2, 3, 4, 5};
    print_deque(d, "d initial");

    auto it = d.insert(d.begin() + 2, 99);
    print_deque(d, "d (insert 99 at index 2)");
    assert(d.size() == 6);
    assert(d[2] == 99);
    assert(*it == 99);
    assert((std::vector<int>(d.begin(), d.end()) ==
            std::vector<int>({1, 2, 99, 3, 4, 5})));

    it = d.insert(d.begin(), 0);
    print_deque(d, "d (insert 0 at begin)");
    assert(d.size() == 7);
    assert(d.front() == 0);
    assert(*it == 0);
    assert((std::vector<int>(d.begin(), d.end()) ==
            std::vector<int>({0, 1, 2, 99, 3, 4, 5})));

    it = d.insert(d.end(), 100);
    print_deque(d, "d (insert 100 at end)");
    assert(d.size() == 8);
    assert(d.back() == 100);
    assert(*it == 100);
    assert((std::vector<int>(d.begin(), d.end()) ==
            std::vector<int>({0, 1, 2, 99, 3, 4, 5, 100})));

    Marcus::deque<int> d_count = {1, 2, 3};
    d_count.insert(d_count.begin() + 1, 2, 77);
    print_deque(d_count, "d_count (insert 2x77 at index 1)");
    assert(d_count.size() == 5);
    assert((std::vector<int>(d_count.begin(), d_count.end()) ==
            std::vector<int>({1, 77, 77, 2, 3})));

    d_count.insert(d_count.end(), 3, 88);
    print_deque(d_count, "d_count (insert 3x88 at end)");
    assert(d_count.size() == 8);
    assert((std::vector<int>(d_count.begin(), d_count.end()) ==
            std::vector<int>({1, 77, 77, 2, 3, 88, 88, 88})));

    Marcus::deque<int> d_range = {10, 20, 30};
    std::vector<int> to_insert = {100, 101, 102};
    d_range.insert(d_range.begin() + 1, to_insert.begin(), to_insert.end());
    print_deque(d_range, "d_range (insert range at index 1)");
    assert(d_range.size() == 6);
    assert((std::vector<int>(d_range.begin(), d_range.end()) ==
            std::vector<int>({10, 100, 101, 102, 20, 30})));

    Marcus::deque<int> d_il = {1, 2, 3};
    d_il.insert(d_il.begin() + 2, {50, 51});
    print_deque(d_il, "d_il (insert initializer list at index 2)");
    assert(d_il.size() == 5);
    assert((std::vector<int>(d_il.begin(), d_il.end()) ==
            std::vector<int>({1, 2, 50, 51, 3})));

    Marcus::deque<MyClass> mc_emplace_d;
    MyClass::reset_counts();
    mc_emplace_d.emplace_back(1);
    mc_emplace_d.emplace_back(2);
    mc_emplace_d.emplace_back(3);
    print_deque(mc_emplace_d, "mc_emplace_d initial");
    assert(MyClass::s_live_count == 3);

    mc_emplace_d.emplace(mc_emplace_d.begin() + 1, 99);
    print_deque(mc_emplace_d, "mc_emplace_d (emplace 99 at index 1)");
    assert(mc_emplace_d.size() == 4);
    assert(mc_emplace_d[1].id == 99);
    assert(MyClass::s_live_count == 4);

    std::cout << "Insert tests passed.\n";
}

void test_erase() {
    std::cout << "\n--- Testing Erase ---\n";
    Marcus::deque<int> d = {1, 2, 3, 4, 5};
    print_deque(d, "d initial");

    auto it = d.erase(d.begin() + 2);
    print_deque(d, "d (erase index 2)");
    assert(d.size() == 4);
    assert(*it == 4);
    assert((std::vector<int>(d.begin(), d.end()) ==
            std::vector<int>({1, 2, 4, 5})));

    it = d.erase(d.begin());
    print_deque(d, "d (erase begin)");
    assert(d.size() == 3);
    assert(*it == 2);
    assert(
        (std::vector<int>(d.begin(), d.end()) == std::vector<int>({2, 4, 5})));

    it = d.erase(d.end() - 1);
    print_deque(d, "d (erase end-1)");
    assert(d.size() == 2);
    assert(it == d.end());
    assert((std::vector<int>(d.begin(), d.end()) == std::vector<int>({2, 4})));

    Marcus::deque<int> d_range = {10, 20, 30, 40, 50, 60};
    print_deque(d_range, "d_range initial");

    it = d_range.erase(d_range.begin() + 1, d_range.begin() + 4);
    print_deque(d_range, "d_range (erase range [1,4))");
    assert(d_range.size() == 3);
    assert(*it == 50);
    assert((std::vector<int>(d_range.begin(), d_range.end()) ==
            std::vector<int>({10, 50, 60})));

    d_range.erase(d_range.begin(), d_range.end());
    print_deque(d_range, "d_range (erase all)");
    assert(d_range.empty());

    std::cout << "\n--- Testing Erase with MyClass ---\n";
    MyClass::reset_counts();
    Marcus::deque<MyClass> mc_erase_d;
    for (int i = 0; i < 5; ++i) {
        mc_erase_d.emplace_back(i);
    }
    print_deque(mc_erase_d, "mc_erase_d initial");
    assert(MyClass::s_live_count == 5);

    mc_erase_d.erase(mc_erase_d.begin() + 2);
    print_deque(mc_erase_d, "mc_erase_d (erase MC(2))");
    assert(MyClass::s_live_count == 4);

    mc_erase_d.erase(mc_erase_d.begin() + 1, mc_erase_d.end() - 1);
    print_deque(mc_erase_d, "mc_erase_d (erase MC(1), MC(3))");
    assert(MyClass::s_live_count == 2);

    mc_erase_d.clear();
    assert(MyClass::s_live_count == 0);
    assert(MyClass::s_constructed == MyClass::s_destroyed);

    std::cout << "Erase tests passed.\n";
}

void test_assign() {
    std::cout << "\n--- Testing Assign ---\n";
    Marcus::deque<int> d_assign = {1, 2, 3};
    print_deque(d_assign, "d_assign initial");

    d_assign.assign(5, 100);
    print_deque(d_assign, "d_assign (assign 5x100)");
    assert(d_assign.size() == 5);
    for (int i = 0; i < 5; ++i) {
        assert(d_assign[i] == 100);
    }

    std::vector<int> v_assign = {10, 20, 30};
    d_assign.assign(v_assign.begin(), v_assign.end());
    print_deque(d_assign, "d_assign (assign from vector)");
    assert(d_assign.size() == 3);
    assert(d_assign[0] == 10 && d_assign[1] == 20 && d_assign[2] == 30);

    std::cout << "Assign tests passed.\n";
}

void test_assignment_operators() {
    std::cout << "\n--- Testing Assignment Operators ---\n";
    Marcus::deque<int> d1 = {1, 2, 3};
    Marcus::deque<int> d2;

    d2 = d1;
    print_deque(d1, "d1");
    print_deque(d2, "d2 (copy assigned from d1)");
    assert(d2.size() == d1.size());
    assert(std::equal(d1.begin(), d1.end(), d2.begin()));

    Marcus::deque<int> d3 = {10, 20, 30, 40};
    Marcus::deque<int> d4 = {100, 200};
    print_deque(d3, "d3 initial");
    print_deque(d4, "d4 initial");

    d4 = std::move(d3);
    print_deque(d4, "d4 (move assigned from d3)");
    print_deque(d3, "d3 (after move assignment)");
    assert(d4.size() == 4);
    assert(d4[0] == 10 && d4[3] == 40);
    assert(d3.empty());

    Marcus::deque<int> d5 = {1, 2, 3};
    d5 = {5, 6, 7, 8};
    print_deque(d5, "d5 (initializer list assigned)");
    assert(d5.size() == 4);
    assert(d5[0] == 5 && d5[3] == 8);

    std::cout << "Assignment operators tests passed.\n";
}

void test_swap() {
    std::cout << "\n--- Testing Swap ---\n";
    Marcus::deque<int> d1 = {1, 2, 3};
    Marcus::deque<int> d2 = {10, 20, 30, 40};
    print_deque(d1, "d1 before swap");
    print_deque(d2, "d2 before swap");

    d1.swap(d2);
    print_deque(d1, "d1 after member swap");
    print_deque(d2, "d2 after member swap");
    assert(d1.size() == 4 && d1[0] == 10);
    assert(d2.size() == 3 && d2[0] == 1);

    std::swap(d1, d2);
    print_deque(d1, "d1 after non-member swap");
    print_deque(d2, "d2 after non-member swap");
    assert(d1.size() == 3 && d1[0] == 1);
    assert(d2.size() == 4 && d2[0] == 10);

    std::cout << "Swap tests passed.\n";
}

void test_comparison_operators() {
    std::cout << "\n--- Testing Comparison Operators ---\n";
    Marcus::deque<int> d1 = {1, 2, 3};
    Marcus::deque<int> d2 = {1, 2, 3};
    Marcus::deque<int> d3 = {1, 2, 4};
    Marcus::deque<int> d4 = {1, 2};
    Marcus::deque<int> d5 = {1, 2, 3, 4};

    assert(d1 == d2);
    assert(!(d1 == d3));
    assert(!(d1 == d4));

    assert(d1 != d3);
    assert(d1 != d4);

    assert(d1 < d3);
    assert(d4 < d1);
    assert(!(d1 < d2));

    assert(d3 > d1);
    assert(d1 > d4);
    assert(!(d1 > d2));

    assert(d1 <= d2);
    assert(d1 <= d3);
    assert(!(d3 <= d1));

    assert(d1 >= d2);
    assert(d3 >= d1);
    assert(!(d1 >= d3));

#if __cpp_lib_three_way_comparison
    std::cout << "C++20 three-way comparison enabled.\n";
    assert((d1 <=> d2) == 0);
    assert((d1 <=> d3) < 0);
    assert((d3 <=> d1) > 0);
    assert((d4 <=> d1) < 0);
    assert((d1 <=> d4) > 0);
#else
    std::cout << "C++20 three-way comparison not enabled. Using traditional "
                 "operators.\n";
#endif

    std::cout << "Comparison operators tests passed.\n";
}

void test_myclass_resource_management() {
    std::cout << "\n--- Testing MyClass Resource Management ---\n";
    MyClass::reset_counts();
    {
        Marcus::deque<MyClass> d;
        for (int i = 0; i < 10; ++i) {
            d.emplace_back(i);
        }
        print_deque(d, "MyClass deque");
        assert(MyClass::s_live_count == 10);

        d.pop_front();
        d.pop_back();
        assert(MyClass::s_live_count == 8);

        d.clear();
        assert(MyClass::s_live_count == 0);
    }
    assert(MyClass::s_live_count == 0);
    assert(MyClass::s_constructed == MyClass::s_destroyed);
    std::cout
        << "MyClass resource management (basic) tests passed. Constructed: "
        << MyClass::s_constructed << ", Destroyed: " << MyClass::s_destroyed
        << std::endl;

    MyClass::reset_counts();
    {
        Marcus::deque<MyClass> d1;
        for (int i = 0; i < 5; ++i) {
            d1.push_back(MyClass(i));
        }
        assert(MyClass::s_live_count == 5);

        Marcus::deque<MyClass> d2 = d1;
        assert(MyClass::s_live_count == 10);

        Marcus::deque<MyClass> d3;
        d3 = d1;
        assert(MyClass::s_live_count == 15);

        Marcus::deque<MyClass> d4 = std::move(d1);
        assert(MyClass::s_live_count == 15);

        Marcus::deque<MyClass> d5;
        d5 = std::move(d2);
        assert(MyClass::s_live_count == 15);
    }
    std::cout
        << "MyClass resource management (copy/move) tests passed. Constructed: "
        << MyClass::s_constructed << ", Destroyed: " << MyClass::s_destroyed
        << std::endl;
    assert(MyClass::s_live_count == 0);
    assert(MyClass::s_constructed == MyClass::s_destroyed);
}

int main() {
    std::cout << "Starting Marcus::deque tests...\n";

    test_constructors();
    test_element_access();
    test_iterators();
    test_capacity();
    test_push_pop();
    test_insert();
    test_erase();
    test_assign();
    test_assignment_operators();
    test_swap();
    test_comparison_operators();
    test_myclass_resource_management();

    std::cout << "\nAll Marcus::deque tests passed successfully!\n";

    return 0;
}

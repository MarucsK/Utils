#include <adaptors/stack.hpp>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct MyStruct {
    int id;
    std::string name;

    MyStruct(int i, std::string s) : id(i), name(std::move(s)) {}

    bool operator==(const MyStruct &other) const {
        return id == other.id && name == other.name;
    }
};

void print_test_header(const std::string &title) {
    std::cout << "\n--- " << title << " ---\n";
}

void test_constructors() {
    print_test_header("Test: Constructors and Basic State");

    Marcus::stack<int> s1;
    assert(s1.empty());
    assert(s1.size() == 0);
    std::cout << "Default constructor: OK\n";

    Marcus::deque<int> d1 = {1, 2, 3};
    Marcus::stack<int> s2(d1);
    assert(!s2.empty());
    assert(s2.size() == 3);
    assert(s2.top() == 3);
    std::cout << "Container copy constructor: OK\n";

    Marcus::deque<int> d2 = {4, 5, 6};
    Marcus::stack<int> s3(std::move(d2));
    assert(!s3.empty());
    assert(s3.size() == 3);
    assert(s3.top() == 6);
    assert(d2.empty());
    std::cout << "Container move constructor: OK\n";

    Marcus::stack<int> s4(s3);
    assert(s4.size() == 3);
    assert(s4.top() == 6);
    assert(s3.size() == 3);
    std::cout << "Copy constructor: OK\n";

    Marcus::stack<int> s5(std::move(s4));
    assert(s5.size() == 3);
    assert(s5.top() == 6);
    assert(s4.empty());
    std::cout << "Move constructor: OK\n";
}

void test_lifo_operations() {
    print_test_header("Test: LIFO Operations (push, pop, top)");
    Marcus::stack<std::string> s;

    s.push("one");
    assert(s.size() == 1);
    assert(s.top() == "one");

    s.push("two");
    assert(s.size() == 2);
    assert(s.top() == "two");

    s.push("three");
    assert(s.size() == 3);
    assert(s.top() == "three");
    std::cout << "Push operations: OK\n";

    s.pop();
    assert(s.size() == 2);
    assert(s.top() == "two");

    s.pop();
    assert(s.size() == 1);
    assert(s.top() == "one");

    const auto &const_s = s;
    assert(const_s.top() == "one");
    std::cout << "Const top(): OK\n";

    s.pop();
    assert(s.size() == 0);
    assert(s.empty());
    std::cout << "Pop operations: OK\n";
}

void test_assignment_operators() {
    print_test_header("Test: Assignment Operators");
    Marcus::stack<int> s1;
    s1.push(10);
    s1.push(20);

    Marcus::stack<int> s2;
    s2.push(99);

    s2 = s1;
    assert(s2.size() == 2);
    assert(s2.top() == 20);
    assert(s1.size() == 2);
    std::cout << "Copy assignment: OK\n";

    Marcus::stack<int> s3;
    s3 = std::move(s1);
    assert(s3.size() == 2);
    assert(s3.top() == 20);
    assert(s1.empty());
    std::cout << "Move assignment: OK\n";
}

void test_emplace() {
    print_test_header("Test: Emplace");
    Marcus::stack<MyStruct> s;

    s.emplace(1, "hello");
    assert(s.size() == 1);
    assert(s.top().id == 1 && s.top().name == "hello");

    s.emplace(2, "world");
    assert(s.size() == 2);
    assert(s.top() == MyStruct(2, "world"));
    std::cout << "Emplace: OK\n";
}

void test_swap() {
    print_test_header("Test: Swap");
    Marcus::stack<int> s1;
    s1.push(1);
    s1.push(2);

    Marcus::stack<int> s2;
    s2.push(100);
    s2.push(200);
    s2.push(300);

    s1.swap(s2);
    assert(s1.size() == 3 && s1.top() == 300);
    assert(s2.size() == 2 && s2.top() == 2);
    std::cout << "Member swap: OK\n";

    swap(s1, s2);
    assert(s1.size() == 2 && s1.top() == 2);
    assert(s2.size() == 3 && s2.top() == 300);
    std::cout << "Non-member swap: OK\n";
}

void test_relational_operators() {
    print_test_header("Test: Relational Operators");
    Marcus::stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);

    Marcus::stack<int> s2;
    s2.push(1);
    s2.push(2);
    s2.push(3);

    Marcus::stack<int> s3;
    s3.push(1);
    s3.push(2);
    s3.push(4);

    Marcus::stack<int> s4;
    s4.push(1);
    s4.push(2);

    assert(s1 == s2);
    std::cout << "Operator== (equal): OK\n";

    assert(s1 != s3);
    std::cout << "Operator!= (not equal): OK\n";

    assert(s1 < s3);
    std::cout << "Operator< (less): OK\n";

    assert(s3 > s1);
    std::cout << "Operator> (greater): OK\n";

    assert(s4 < s1);
    std::cout << "Operator< (prefix): OK\n";

    assert(s1 <= s2);
    assert(s1 <= s3);
    std::cout << "Operator<= (less or equal): OK\n";

    assert(s1 >= s2);
    assert(s3 >= s1);
    std::cout << "Operator>= (greater or equal): OK\n";
}

void test_with_std_vector() {
    print_test_header("Test: With std::vector as underlying container");

    using vector_stack = Marcus::stack<int, std::vector<int>>;

    vector_stack s;
    s.push(10);
    s.push(20);

    assert(!s.empty());
    assert(s.size() == 2);
    assert(s.top() == 20);

    s.pop();
    assert(s.size() == 1);
    assert(s.top() == 10);
    std::cout << "Stack with std::vector works correctly: OK\n";
}

int main() {
    test_constructors();
    test_lifo_operations();
    test_assignment_operators();
    test_emplace();
    test_swap();
    test_relational_operators();
    test_with_std_vector();

    std::cout << "\n=========================" << std::endl;
    std::cout << "All tests passed successfully!" << std::endl;
    std::cout << "=========================" << std::endl;

    return 0;
}

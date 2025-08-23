#include <adaptors/queue.hpp>
#include <cassert>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

int tests_passed = 0;
int tests_failed = 0;

#define ASSERT_TRUE(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
        } else { \
            tests_failed++; \
            std::cerr << "Assertion FAILED: (" #condition ") at " << __FILE__ \
                      << ":" << __LINE__ << " - " << message << std::endl; \
        } \
    } while (0)

struct TestStruct {
    int id;
    std::string name;

    TestStruct(int i, std::string n) : id(i), name(std::move(n)) {}

    bool operator==(const TestStruct &other) const {
        return id == other.id && name == other.name;
    }
};

void test_default_constructor_and_capacity() {
    std::cout << "--- Running Test: Default Constructor, empty(), size() ---\n";
    Marcus::queue<int> q;
    ASSERT_TRUE(q.empty(), "A default-constructed queue should be empty.");
    ASSERT_TRUE(q.size() == 0,
                "A default-constructed queue should have size 0.");
}

void test_modifiers_and_accessors() {
    std::cout << "--- Running Test: push(), pop(), front(), back() ---\n";
    Marcus::queue<std::string> q;

    std::string s1 = "hello";
    q.push(s1);
    ASSERT_TRUE(q.size() == 1, "Size should be 1 after one push.");
    ASSERT_TRUE(q.front() == "hello", "front() should be 'hello'.");
    ASSERT_TRUE(q.back() == "hello", "back() should be 'hello'.");

    q.push("world");
    ASSERT_TRUE(q.size() == 2, "Size should be 2 after two pushes.");
    ASSERT_TRUE(q.front() == "hello", "front() should remain 'hello'.");
    ASSERT_TRUE(q.back() == "world", "back() should now be 'world'.");

    q.front() = "greetings";
    ASSERT_TRUE(q.front() == "greetings", "front() should be modifiable.");

    q.pop();
    ASSERT_TRUE(q.size() == 1, "Size should be 1 after one pop.");
    ASSERT_TRUE(q.front() == "world", "front() should be 'world' after pop.");
    ASSERT_TRUE(q.back() == "world",
                "back() should still be 'world' after pop.");

    q.pop();
    ASSERT_TRUE(q.empty(), "Queue should be empty after popping all elements.");
    ASSERT_TRUE(q.size() == 0, "Size should be 0 after popping all elements.");
}

void test_emplace() {
    std::cout << "--- Running Test: emplace() ---\n";
    Marcus::queue<TestStruct> q;

    q.emplace(1, "one");
    ASSERT_TRUE(q.size() == 1, "Size should be 1 after emplace.");
    ASSERT_TRUE(q.back().id == 1 && q.back().name == "one",
                "emplace() should correctly construct the object in place.");

    q.emplace(2, "two");
    ASSERT_TRUE(q.size() == 2, "Size should be 2 after second emplace.");
    ASSERT_TRUE(q.front().id == 1,
                "front() should be the first emplaced object.");
    ASSERT_TRUE(q.back().id == 2,
                "back() should be the second emplaced object.");
}

void test_copy_constructor() {
    std::cout << "--- Running Test: Copy Constructor ---\n";
    Marcus::queue<int> q1;
    q1.push(10);
    q1.push(20);
    q1.push(30);

    Marcus::queue<int> q2(q1);
    ASSERT_TRUE(q1.size() == 3,
                "Original queue size should be unchanged after copy.");
    ASSERT_TRUE(q2.size() == 3, "Copied queue should have the same size.");
    ASSERT_TRUE(q1.front() == q2.front() && q1.back() == q2.back(),
                "Copied queue should have same front and back.");

    q2.pop();
    ASSERT_TRUE(q1.size() == 3, "Modifying copy should not affect original.");
    ASSERT_TRUE(q1.front() == 10,
                "Original queue's front should be unchanged.");
    ASSERT_TRUE(q2.front() == 20, "Copy's front should change after pop.");
}

void test_move_constructor() {
    std::cout << "--- Running Test: Move Constructor ---\n";
    Marcus::queue<int> q1;
    q1.push(100);
    q1.push(200);

    Marcus::queue<int> q2(std::move(q1));
    ASSERT_TRUE(q2.size() == 2,
                "Moved-to queue should have the original size.");
    ASSERT_TRUE(q2.front() == 100,
                "Moved-to queue should have the correct front element.");
    ASSERT_TRUE(q1.empty(), "Moved-from queue should be empty.");
}

void test_copy_assignment() {
    std::cout << "--- Running Test: Copy Assignment Operator ---\n";
    Marcus::queue<int> q1;
    q1.push(1);
    q1.push(2);

    Marcus::queue<int> q2;
    q2.push(99);

    q2 = q1;
    ASSERT_TRUE(
        q1.size() == 2,
        "Original queue size should be unchanged after copy assignment.");
    ASSERT_TRUE(q2.size() == 2, "Assigned queue should have the same size.");
    ASSERT_TRUE(q2.front() == 1,
                "Assigned queue should have the correct front element.");

    q1.pop();
    ASSERT_TRUE(q1.front() == 2, "Original queue front changes after pop.");
    ASSERT_TRUE(q2.front() == 1, "Assigned queue front should not change.");
}

void test_move_assignment() {
    std::cout << "--- Running Test: Move Assignment Operator ---\n";
    Marcus::queue<int> q1;
    q1.push(11);
    q1.push(22);

    Marcus::queue<int> q2;
    q2.push(99);

    q2 = std::move(q1);
    ASSERT_TRUE(q2.size() == 2,
                "Moved-to queue should have the original size.");
    ASSERT_TRUE(q2.front() == 11,
                "Moved-to queue should have the correct front element.");
    ASSERT_TRUE(q1.empty(),
                "Moved-from queue should be empty after move assignment.");
}

void test_swap() {
    std::cout << "--- Running Test: swap() ---\n";
    Marcus::queue<int> q1;
    q1.push(1);
    q1.push(2);

    Marcus::queue<int> q2;
    q2.push(101);
    q2.push(102);
    q2.push(103);

    q1.swap(q2);
    ASSERT_TRUE(q1.size() == 3, "q1 size should be 3 after swap.");
    ASSERT_TRUE(q2.size() == 2, "q2 size should be 2 after swap.");
    ASSERT_TRUE(q1.front() == 101, "q1 front should be 101 after swap.");
    ASSERT_TRUE(q2.front() == 1, "q2 front should be 1 after swap.");

    using std::swap;
    swap(q1, q2);
    ASSERT_TRUE(q1.size() == 2, "q1 size should be 2 after non-member swap.");
    ASSERT_TRUE(q2.size() == 3, "q2 size should be 3 after non-member swap.");
    ASSERT_TRUE(q1.front() == 1, "q1 front should be 1 after non-member swap.");
    ASSERT_TRUE(q2.front() == 101,
                "q2 front should be 101 after non-member swap.");
}

void test_comparison_operators() {
    std::cout << "--- Running Test: Comparison Operators ---\n";
    Marcus::queue<int> q1, q2, q3, q4;

    q1.push(10);
    q1.push(20);
    q2.push(10);
    q2.push(20);
    q3.push(10);
    q3.push(30);
    q4.push(10);

#if __cpp_lib_three_way_comparison
    std::cout << "  (Testing with <=>)\n";
    ASSERT_TRUE((q1 <=> q2) == std::strong_ordering::equal,
                "q1 <=> q2 should be equal.");
    ASSERT_TRUE((q1 <=> q3) == std::strong_ordering::less,
                "q1 <=> q3 should be less.");
    ASSERT_TRUE((q3 <=> q1) == std::strong_ordering::greater,
                "q3 <=> q1 should be greater.");
    ASSERT_TRUE((q4 <=> q1) == std::strong_ordering::less,
                "q4 <=> q1 should be less (shorter sequence).");
#else
    std::cout << "  (Testing with ==, !=, <, <=, >, >=)\n";
    ASSERT_TRUE(q1 == q2, "q1 should be equal to q2.");
    ASSERT_TRUE(!(q1 != q2), "q1 should not be unequal to q2.");

    ASSERT_TRUE(q1 != q3, "q1 should be unequal to q3.");
    ASSERT_TRUE(!(q1 == q3), "q1 should not be equal to q3.");

    ASSERT_TRUE(q1 < q3, "q1 should be less than q3.");
    ASSERT_TRUE(q1 <= q3, "q1 should be less than or equal to q3.");
    ASSERT_TRUE(q3 > q1, "q3 should be greater than q1.");
    ASSERT_TRUE(q3 >= q1, "q3 should be greater than or equal to q1.");

    ASSERT_TRUE(q4 < q1, "A shorter queue should be less than a longer one "
                         "with the same prefix.");
    ASSERT_TRUE(q1 > q4, "A longer queue should be greater than a shorter one "
                         "with the same prefix.");
#endif
}

void test_with_different_container() {
    std::cout
        << "--- Running Test: With std::list as underlying container ---\n";
    Marcus::queue<int, std::list<int>> q;

    ASSERT_TRUE(q.empty(), "Queue with std::list should be empty initially.");
    q.push(100);
    q.push(200);
    ASSERT_TRUE(q.size() == 2, "Size should be 2 after pushes.");
    ASSERT_TRUE(q.front() == 100,
                "front() should work correctly with std::list.");
    q.pop();
    ASSERT_TRUE(q.front() == 200,
                "pop() should work correctly with std::list.");
    q.pop();
    ASSERT_TRUE(q.empty(), "Queue should be empty finally.");
}

int main() {
    std::cout << "=======================================\n";
    std::cout << "     Starting Marcus::queue tests      \n";
    std::cout << "=======================================\n\n";

    test_default_constructor_and_capacity();
    test_modifiers_and_accessors();
    test_emplace();
    test_copy_constructor();
    test_move_constructor();
    test_copy_assignment();
    test_move_assignment();
    test_swap();
    test_comparison_operators();
    test_with_different_container();

    std::cout << "\n=======================================\n";
    std::cout << "           Test Results                \n";
    std::cout << "=======================================\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
    std::cout << "=======================================\n\n";

    if (tests_failed == 0) {
        std::cout << "Congratulations! All Marcus::queue tests passed!\n";
        return 0;
    } else {
        std::cerr
            << "Some Marcus::queue tests failed. Please review the output.\n";
        return 1;
    }
}

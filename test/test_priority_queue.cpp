#include <adaptors/priority_queue.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>

#define TEST_CASE(name) \
    void test_##name() { \
        std::cout << "Running test: " << #name << "..." << std::endl;

#define END_TEST_CASE(name) \
    std::cout << "Test " << #name << " PASSED." << std::endl; \
    }

void check(bool condition, const std::string &message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << std::endl;
        assert(false && "Test failed!"); // Abort on first failure
    }
}

// --- Test Cases ---

TEST_CASE(DefaultConstructorAndBasicOps) {
    Marcus::priority_queue<int> pq;
    check(pq.empty(), "Default constructed queue should be empty.");
    check(pq.size() == 0, "Default constructed queue size should be 0.");

    pq.push(10);
    check(!pq.empty(), "Queue should not be empty after push.");
    check(pq.size() == 1, "Queue size should be 1 after one push.");
    check(pq.top() == 10, "Top should be 10.");

    pq.push(20);
    check(pq.top() == 20, "Top should be 20 after pushing 20.");
    pq.push(5);
    check(pq.top() == 20, "Top should still be 20 after pushing 5.");

    pq.pop();
    check(pq.top() == 10, "Top should be 10 after popping 20.");
    pq.pop();
    check(pq.top() == 5, "Top should be 5 after popping 10.");
    pq.pop();
    check(pq.empty(), "Queue should be empty after popping all elements.");
    check(pq.size() == 0, "Queue size should be 0 after popping all elements.");
}
END_TEST_CASE(DefaultConstructorAndBasicOps)

TEST_CASE(MinHeapWithGreaterCompare) {
    Marcus::priority_queue<int, Marcus::vector<int>, std::greater<int>> min_pq;
    check(min_pq.empty(), "Min-heap should be empty.");

    min_pq.push(10);
    min_pq.push(5);
    min_pq.push(20);
    min_pq.push(1);

    check(min_pq.top() == 1, "Min-heap top should be 1.");
    check(min_pq.size() == 4, "Min-heap size should be 4.");

    min_pq.pop();
    check(min_pq.top() == 5, "Min-heap top should be 5 after popping 1.");
    min_pq.pop();
    check(min_pq.top() == 10, "Min-heap top should be 10 after popping 5.");
    min_pq.pop();
    check(min_pq.top() == 20, "Min-heap top should be 20 after popping 10.");
    min_pq.pop();
    check(min_pq.empty(), "Min-heap should be empty after popping all.");
}
END_TEST_CASE(MinHeapWithGreaterCompare)

TEST_CASE(PushRValueAndEmplace) {
    Marcus::priority_queue<std::string> pq_str;

    // push (rvalue)
    std::string s1 = "apple";
    pq_str.push(std::move(s1));
    check(pq_str.top() == "apple", "push(rvalue): top is apple.");
    check(s1.empty(),
          "push(rvalue): moved-from string should be empty."); // std::string
                                                               // specific

    // emplace
    pq_str.emplace("banana");
    check(pq_str.top() == "banana", "emplace: top is banana.");
    pq_str.emplace("cherry");
    check(pq_str.top() == "cherry", "emplace: top is cherry.");
    pq_str.emplace("date");
    check(pq_str.top() == "date", "emplace: top is date.");

    check(pq_str.size() == 4, "emplace: size is 4.");
    pq_str.pop(); // pop date
    check(pq_str.top() == "cherry", "emplace: top after pop.");
}
END_TEST_CASE(PushRValueAndEmplace)

TEST_CASE(CopyAndMoveSemantics) {
    Marcus::priority_queue<int> original_pq;
    original_pq.push(1);
    original_pq.push(5);
    original_pq.push(2); // original_pq: {5, 2, 1}

    // Copy Constructor
    Marcus::priority_queue<int> copied_pq = original_pq;
    check(copied_pq.size() == 3, "Copy constructor: size mismatch.");
    check(copied_pq.top() == 5, "Copy constructor: top mismatch.");
    original_pq.pop(); // Modify original: original_pq: {2, 1}
    check(copied_pq.top() == 5,
          "Copy constructor: copied queue should be independent.");
    check(copied_pq.size() == 3,
          "Copy constructor: copied queue size unchanged.");

    // Copy Assignment
    Marcus::priority_queue<int> assigned_pq;
    assigned_pq.push(100);
    assigned_pq = original_pq; // original_pq is now {2, 1}
    check(assigned_pq.size() == 2, "Copy assignment: size mismatch.");
    check(assigned_pq.top() == 2, "Copy assignment: top mismatch.");
    original_pq.pop(); // original_pq: {1}
    check(assigned_pq.top() == 2,
          "Copy assignment: assigned queue should be independent.");

    // Move Constructor
    Marcus::priority_queue<int> moved_pq =
        std::move(original_pq); // original_pq is now {1}, then moved.
                                // original_pq becomes empty.
    check(moved_pq.size() == 1, "Move constructor: size mismatch.");
    check(moved_pq.top() == 1, "Move constructor: top mismatch.");
    check(original_pq.empty(), "Move constructor: original should be empty.");

    // Move Assignment
    Marcus::priority_queue<int> moved_assigned_pq;
    moved_assigned_pq.push(999);
    moved_assigned_pq =
        std::move(copied_pq); // copied_pq is now {5, 2, 1}, then moved.
                              // copied_pq becomes empty.
    check(moved_assigned_pq.size() == 3, "Move assignment: size mismatch.");
    check(moved_assigned_pq.top() == 5, "Move assignment: top mismatch.");
    check(copied_pq.empty(),
          "Move assignment: original (copied_pq) should be empty.");
}
END_TEST_CASE(CopyAndMoveSemantics)

TEST_CASE(SwapFunction) {
    Marcus::priority_queue<int> pq1;
    pq1.push(1);
    pq1.push(5);
    pq1.push(2); // Top: 5, Size: 3

    Marcus::priority_queue<int> pq2;
    pq2.push(10);
    pq2.push(20); // Top: 20, Size: 2

    // Member swap
    pq1.swap(pq2);
    check(pq1.size() == 2, "Member swap: pq1 size after swap.");
    check(pq1.top() == 20, "Member swap: pq1 top after swap.");
    check(pq2.size() == 3, "Member swap: pq2 size after swap.");
    check(pq2.top() == 5, "Member swap: pq2 top after swap.");

    // Friend swap (using ADL)
    Marcus::priority_queue<int> pq3;
    pq3.push(100);  // Top: 100, Size: 1

    swap(pq1, pq3); // pq1 was {20, 10}, pq3 was {100}
    check(pq1.size() == 1, "Friend swap: pq1 size after swap.");
    check(pq1.top() == 100, "Friend swap: pq1 top after swap.");
    check(pq3.size() == 2, "Friend swap: pq3 size after swap.");
    check(pq3.top() == 20, "Friend swap: pq3 top after swap.");
}
END_TEST_CASE(SwapFunction)

TEST_CASE(ConstructorWithCompareOnly) {
    Marcus::priority_queue<int, Marcus::vector<int>, std::greater<int>> min_pq{
        std::greater<int>()};
    min_pq.push(5);
    min_pq.push(1);
    check(min_pq.top() == 1, "Constructor with compare only: top.");
    check(min_pq.size() == 2, "Constructor with compare only: size.");
}
END_TEST_CASE(ConstructorWithCompareOnly)

TEST_CASE(ConstructorsWithContainer) {
    Marcus::vector<int> initial_data = {10, 30, 20, 5}; // Max-heap: 30

    // Constructor with Compare and const Container&
    Marcus::priority_queue<int> pq1(std::less<int>(), initial_data);
    check(pq1.size() == 4, "Ctor (Compare, const Cont&): size.");
    check(pq1.top() == 30, "Ctor (Compare, const Cont&): top.");

    // Constructor with Compare and Container&&
    Marcus::priority_queue<int> pq2(
        std::less<int>(), Marcus::vector<int>{1, 2, 3}); // Max-heap: 3
    check(pq2.size() == 3, "Ctor (Compare, Cont&&): size.");
    check(pq2.top() == 3, "Ctor (Compare, Cont&&): top.");

    // Constructor with const Container& (default compare)
    Marcus::priority_queue<int> pq3(
        initial_data); // initial_data is {10, 30, 20, 5}, Max-heap: 30
    check(pq3.size() == 4, "Ctor (const Cont&): size.");
    check(pq3.top() == 30, "Ctor (const Cont&): top.");

    // Constructor with Container&& (default compare)
    Marcus::priority_queue<int> pq4(
        Marcus::vector<int>{100, 50, 150}); // Max-heap: 150
    check(pq4.size() == 3, "Ctor (Cont&&): size.");
    check(pq4.top() == 150, "Ctor (Cont&&): top.");
}
END_TEST_CASE(ConstructorsWithContainer)

TEST_CASE(ConstructorsWithInputIteratorRange) {
    std::vector<int> data = {1, 8, 3, 6, 2, 9}; // Max-heap: 9

    // Constructor with InputIt range (default compare)
    Marcus::priority_queue<int> pq1(data.begin(), data.end());
    check(pq1.size() == 6, "Ctor (It, It): size.");
    check(pq1.top() == 9, "Ctor (It, It): top.");

    // Constructor with InputIt range and Compare
    Marcus::priority_queue<int, Marcus::vector<int>, std::greater<int>> pq2(
        data.begin(), data.end(), std::greater<int>()); // Min-heap: 1
    check(pq2.size() == 6, "Ctor (It, It, Compare): size.");
    check(pq2.top() == 1, "Ctor (It, It, Compare): top.");

    // Constructor with InputIt range, Compare, and const Container&
    Marcus::vector<int> base_cont = {100, 200}; // Max-heap: 200
    Marcus::priority_queue<int> pq3(
        data.begin(), data.end(), std::less<int>(),
        base_cont); // Combined: {1,8,3,6,2,9,100,200}. Max-heap: 200
    check(pq3.size() == 8,
          "Ctor (It, It, Compare, const Cont&): size."); // 2 from base_cont + 6
                                                         // from data
    check(pq3.top() == 200, "Ctor (It, It, Compare, const Cont&): top.");

    // Constructor with InputIt range, Compare, and Container&&
    Marcus::priority_queue<int> pq4(
        data.begin(), data.end(), std::less<int>(),
        Marcus::vector<int>{
            300, 400}); // Combined: {1,8,3,6,2,9,300,400}. Max-heap: 400
    check(pq4.size() == 8, "Ctor (It, It, Compare, Cont&&): size.");
    check(pq4.top() == 400, "Ctor (It, It, Compare, Cont&&): top.");
}
END_TEST_CASE(ConstructorsWithInputIteratorRange)

TEST_CASE(ConstructorsWithInitializerList) {
    // Constructor with initializer_list (default compare)
    Marcus::priority_queue<int> pq1 = {10, 5, 20, 15}; // Max-heap: 20
    check(pq1.size() == 4, "Ctor (IL): size.");
    check(pq1.top() == 20, "Ctor (IL): top.");

    // Constructor with initializer_list and Compare
    Marcus::priority_queue<int, Marcus::vector<int>, std::greater<int>> pq2(
        {10, 5, 20, 15}, std::greater<int>()); // Min-heap: 5
    check(pq2.size() == 4, "Ctor (IL, Compare): size.");
    check(pq2.top() == 5, "Ctor (IL, Compare): top.");

    // Constructor with initializer_list, Compare, and const Container&
    Marcus::vector<int> base_cont = {1, 2}; // Max-heap: 2
    Marcus::priority_queue<int> pq3(
        {10, 5, 20}, std::less<int>(),
        base_cont); // Combined: {1,2,10,5,20}. Max-heap: 20
    check(pq3.size() == 5,
          "Ctor (IL, Compare, const Cont&): size."); // 2 from base_cont + 3
                                                     // from IL
    check(pq3.top() == 20, "Ctor (IL, Compare, const Cont&): top.");

    // Constructor with initializer_list, Compare, and Container&&
    Marcus::priority_queue<int> pq4(
        {10, 5, 20}, std::less<int>(),
        Marcus::vector<int>{30, 40}); // Combined: {30,40,10,5,20}. Max-heap: 40
    check(pq4.size() == 5, "Ctor (IL, Compare, Cont&&): size.");
    check(pq4.top() == 40, "Ctor (IL, Compare, Cont&&): top.");
}
END_TEST_CASE(ConstructorsWithInitializerList)

TEST_CASE(EmptyAndSize) {
    Marcus::priority_queue<int> pq;
    check(pq.empty(), "Initially empty.");
    check(pq.size() == 0, "Initially size 0.");

    pq.push(1);
    check(!pq.empty(), "Not empty after push.");
    check(pq.size() == 1, "Size 1 after push.");

    pq.pop();
    check(pq.empty(), "Empty after pop.");
    check(pq.size() == 0, "Size 0 after pop.");

    // Test with multiple elements
    for (int i = 0; i < 10; ++i) {
        pq.push(i);
    }
    check(!pq.empty(), "Not empty after multiple pushes.");
    check(pq.size() == 10, "Size 10 after multiple pushes.");

    for (int i = 0; i < 10; ++i) {
        pq.pop();
    }
    check(pq.empty(), "Empty after multiple pops.");
    check(pq.size() == 0, "Size 0 after multiple pops.");
}
END_TEST_CASE(EmptyAndSize)

int main() {
    std::cout << "Starting Marcus::priority_queue tests..." << std::endl;

    test_DefaultConstructorAndBasicOps();
    test_MinHeapWithGreaterCompare();
    test_PushRValueAndEmplace();
    test_CopyAndMoveSemantics();
    test_SwapFunction();
    test_ConstructorWithCompareOnly();
    test_ConstructorsWithContainer();
    test_ConstructorsWithInputIteratorRange();
    test_ConstructorsWithInitializerList();
    test_EmptyAndSize();

    std::cout << "\nAll Marcus::priority_queue tests completed successfully!"
              << std::endl;
    return 0;
}

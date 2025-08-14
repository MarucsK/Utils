#include <containers/forward_list.hpp>
#include <iostream>

template <typename T, typename Alloc>
void print_list(const std::string &label,
                const Marcus::forward_list<T, Alloc> &list) {
    std::cout << label << ": [";
    bool first = true;
    for (const auto &val: list) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << val;
        first = false;
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "--- Marcus::forward_list Tests ---\n\n";

    Marcus::forward_list<int> list1;
    print_list("List 1 (empty)", list1);
    std::cout << "Is List 1 empty? " << (list1.empty() ? "Yes" : "No") << "\n";

    list1.push_front(30);
    list1.emplace_front(20);
    list1.push_front(10);
    print_list("List 1 (after push_front/emplace_front)", list1);
    std::cout << "Front of List 1: " << list1.front() << "\n";

    list1.pop_front();
    print_list("List 1 (after pop_front)", list1);
    std::cout << "Front of List 1: " << list1.front() << "\n";

    list1.pop_front();
    list1.pop_front();
    print_list("List 1 (after more pop_fronts)", list1);
    std::cout << "Is List 1 empty? " << (list1.empty() ? "Yes" : "No")
              << "\n\n";

    Marcus::forward_list<int> list2 = {1, 2, 3, 4, 5};
    print_list("List 2 (initializer list)", list2);

    list2.assign({10, 20, 30});
    print_list("List 2 (after assign initializer list)", list2);

    list2.assign(3, 99); // assign 3 times 99
    print_list("List 2 (after assign count, value)", list2);

    Marcus::forward_list<int> list3(2, 77);
    print_list("List 3 (constructor count, value)", list3);
    std::cout << "\n";

    Marcus::forward_list<int> list4 = {1, 2, 3};
    print_list("List 4 (initial)", list4);

    list4.insert_after(list4.before_begin(), 0);
    print_list("List 4 (after insert_after(before_begin(), 0))",
               list4); // Expected: 0, 1, 2, 3

    auto it = list4.begin();
    ++it;
    list4.insert_after(it, 5);
    print_list("List 4 (after insert_after(it, 5))",
               list4); // Expected: 0, 1, 5, 2, 3

    list4.insert_after(list4.cbegin(), 2, 100);
    print_list("List 4 (after insert_after(cbegin(), 2, 100))",
               list4); // Expected: 0, 100, 100, 1, 5, 2, 3

    Marcus::forward_list<int> temp_list = {11, 22};
    list4.insert_after(list4.cbegin(), temp_list.begin(), temp_list.end());
    print_list("List 4 (after insert_after(cbegin(), {11, 22}))",
               list4); // Expected: 0, 11, 22, 100, 100, 1, 5, 2, 3

    list4.erase_after(list4.before_begin());
    print_list("List 4 (after erase_after(before_begin()))",
               list4); // Expected: 11, 22, 100, 100, 1, 5, 2, 3

    auto erase_it_start = list4.cbegin();
    ++erase_it_start;
    auto erase_it_end = erase_it_start;
    ++erase_it_end;
    ++erase_it_end;
    ++erase_it_end;

    list4.erase_after(erase_it_start, erase_it_end);
    print_list("List 4 (after erase_after(22, 1))",
               list4); // Expected: 11, 22, 1, 5, 2, 3
    std::cout << "\n";

    Marcus::forward_list<int> list5 = {1, 2, 2, 3, 2, 4, 5, 2};
    print_list("List 5 (initial for remove)", list5);
    size_t removed_count = list5.remove(2);
    print_list("List 5 (after remove(2))", list5); // Expected: 1, 3, 4, 5
    std::cout << "Removed count: " << removed_count << "\n";

    Marcus::forward_list<int> list6 = {10, 11, 12, 13, 14, 15, 16};
    print_list("List 6 (initial for remove_if)", list6);
    removed_count = list6.remove_if([](int n) { return n % 2 != 0; });
    print_list("List 6 (after remove_if(odd))",
               list6); // Expected: 10, 12, 14, 16
    std::cout << "Removed count: " << removed_count << "\n\n";

    // Test 5: splice_after
    Marcus::forward_list<int> list_a = {1, 2, 3};
    Marcus::forward_list<int> list_b = {10, 20, 30};
    print_list("List A (initial)", list_a);
    print_list("List B (initial)", list_b);

    list_a.splice_after(list_a.begin(), list_b);
    print_list("List A (after splice_after(begin(), list_b))",
               list_a); // Expected: 1, 10, 20, 30, 2, 3
    print_list("List B (after splice_after)", list_b); // Expected: empty

    Marcus::forward_list<int> list_c = {100, 200, 300, 400, 500};
    Marcus::forward_list<int> list_d = {1, 2, 3, 4, 5};
    print_list("List C (initial)", list_c);
    print_list("List D (initial)", list_d);

    auto d_it_before_first = list_d.cbegin(); // 指向 1
    ++d_it_before_first;
    auto d_it_after_last = d_it_before_first; // 指向 2
    ++d_it_after_last;

    list_c.splice_after(list_c.begin(), list_d, d_it_before_first,
                        d_it_after_last);
    print_list("List C (after splice_after(begin(), D, (2,3)))",
               list_c); // Expected: 100, 3, 200, 300, 400, 500
    print_list("List D (after splice_after)", list_d); // Expected: 1, 2, 4, 5

    Marcus::forward_list<int> list_e = {1000, 2000};
    Marcus::forward_list<int> list_f = {10, 20, 30};
    print_list("List E (initial)", list_e);
    print_list("List F (initial)", list_f);
    auto f_it = list_f.cbegin();
    ++f_it;
    list_e.splice_after(list_e.begin(), list_f, f_it);
    print_list("List E (after splice_after(begin(), F, 20))",
               list_e); // Expected: 1000, 20, 2000
    print_list("List F (after splice_after)", list_f); // Expected: 10, 30
    std::cout << "\n";

    Marcus::forward_list<int> list7 = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    print_list("List 7 (initial for sort)", list7);
    list7.sort();
    print_list("List 7 (after sort)",
               list7); // Expected: 1, 2, 3, 4, 5, 6, 7, 8, 9

    Marcus::forward_list<int> list7_desc = {5, 2, 8, 1, 9};
    print_list("List 7 (initial for sort desc)", list7_desc);
    list7_desc.sort(std::greater<int>());
    print_list("List 7 (after sort desc)",
               list7_desc); // Expected: 9, 8, 5, 2, 1
    std::cout << "\n";

    Marcus::forward_list<int> list8 = {1, 1, 2, 3, 3, 3, 4, 5, 5, 6, 6, 6, 6};
    print_list("List 8 (initial for unique)", list8);
    size_t unique_removed_count = list8.unique();
    print_list("List 8 (after unique)", list8); // Expected: 1, 2, 3, 4, 5, 6
    std::cout << "Unique removed count: " << unique_removed_count << "\n";

    Marcus::forward_list<std::string> list8_str = {
        "apple", "apple", "banana", "orange", "orange", "orange", "grape"};
    print_list("List 8 (string initial for unique)", list8_str);
    list8_str.unique();
    print_list("List 8 (string after unique)",
               list8_str); // Expected: apple, banana, orange, grape
    std::cout << "\n";

    Marcus::forward_list<int> list9 = {1, 2, 3, 4, 5};
    print_list("List 9 (initial for reverse)", list9);
    list9.reverse();
    print_list("List 9 (after reverse)", list9); // Expected: 5, 4, 3, 2, 1
    std::cout << "\n";

    Marcus::forward_list<int> list10 = {1, 2, 3};
    Marcus::forward_list<int> list11 = {4, 5, 6};
    print_list("List 10 (initial)", list10);
    print_list("List 11 (initial)", list11);
    list10.swap(list11);
    print_list("List 10 (after swap)", list10); // Expected: 4, 5, 6
    print_list("List 11 (after swap)", list11); // Expected: 1, 2, 3

    Marcus::forward_list<int> list12 = {1, 2, 3};
    Marcus::forward_list<int> list13 = {1, 2, 3};
    Marcus::forward_list<int> list14 = {1, 2, 4};
    Marcus::forward_list<int> list15 = {1, 2};

    std::cout << "list12: " << (list12 == list13 ? "==" : "!=")
              << " list13 (expected ==)\n";
    std::cout << "list12: " << (list12 != list14 ? "!=" : "==")
              << " list14 (expected !=)\n";
    std::cout << "list12: " << (list12 < list14 ? "<" : ">=")
              << " list14 (expected <)\n";
    std::cout << "list14: " << (list14 > list12 ? ">" : "<=")
              << " list12 (expected >)\n";
    std::cout << "list12: " << (list12 >= list15 ? ">=" : "<")
              << " list15 (expected >=)\n";
    std::cout << "list15: " << (list15 <= list12 ? "<=" : ">")
              << " list12 (expected <=)\n";
    std::cout << "\n";

    Marcus::forward_list<int> list_orig = {10, 20, 30};
    Marcus::forward_list<int> list_copy_ctor = list_orig;
    print_list("List orig (for copy)", list_orig);
    print_list("List copy_ctor (after copy constructor)", list_copy_ctor);

    Marcus::forward_list<int> list_copy_assign;
    list_copy_assign = list_orig;
    print_list("List copy_assign (after copy assignment)", list_copy_assign);

    list_orig.push_front(5);
    print_list("List orig (after modification)", list_orig);
    print_list("List copy_ctor (after orig modification)", list_copy_ctor);
    print_list("List copy_assign (after orig modification)", list_copy_assign);
    std::cout << "\n";

    Marcus::forward_list<int> list_move_orig = {100, 200, 300};
    print_list("List move_orig (initial)", list_move_orig);
    Marcus::forward_list<int> list_move_ctor = std::move(list_move_orig);
    print_list("List move_ctor (after move constructor)", list_move_ctor);
    print_list("List move_orig (after move constructor, expected empty)",
               list_move_orig);

    Marcus::forward_list<int> list_move_assign_target = {1, 2, 3};
    Marcus::forward_list<int> list_move_assign_source = {4, 5, 6};
    print_list("List move_assign_target (initial)", list_move_assign_target);
    print_list("List move_assign_source (initial)", list_move_assign_source);
    list_move_assign_target = std::move(list_move_assign_source);
    print_list("List move_assign_target (after move assignment)",
               list_move_assign_target);
    print_list(
        "List move_assign_source (after move assignment, expected empty)",
        list_move_assign_source);
    std::cout << "\n";

    std::cout << "--- All Tests Completed ---\n";

    return 0;
}

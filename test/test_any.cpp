#include <cassert> // 用于简单的断言
#include <iostream>
#include <stdexcept> // 尽管 BadAnyCast 继承自 std::exception，但显式包含有助于理解
#include <string>
#include <type_traits>     // 用于 std::decay_t 等
#include <utility/any.hpp> // 包含你的 Marcus::any 实现
#include <vector>

// --- 测试辅助宏和自定义结构体 ---

// 帮助打印测试用例的名称和结果
#define TEST_CASE(name) \
    std::cout << "--- Running Test: " << name << " ---" << std::endl;
#define TEST_PASSED() \
    std::cout << "--- Test PASSED ---" << std::endl << std::endl;
#define TEST_FAILED(msg) \
    { \
        std::cerr << "!!! Test FAILED: " << msg << std::endl; \
        exit(1); \
    }

// 用于测试的自定义结构体，包含移动语义跟踪
struct MyStruct {
    int id;
    std::string name;
    bool moved_from = false; // 用于跟踪对象是否被移动过

    // 默认构造函数
    MyStruct(int i = 0, std::string n = "default") : id(i), name(n) {
        // std::cout << "MyStruct(" << id << ", " << name << ") constructed." <<
        // std::endl;
    }

    // 拷贝构造函数
    MyStruct(const MyStruct &other) : id(other.id), name(other.name) {
        // std::cout << "MyStruct(" << id << ", " << name << ") copy
        // constructed." << std::endl;
    }

    // 移动构造函数
    MyStruct(MyStruct &&other) noexcept
        : id(other.id),
          name(std::move(other.name)) {
        other.id = -1; // 标记源对象已移动
        other.name = "MOVED_FROM";
        other.moved_from = true;
        // std::cout << "MyStruct(" << id << ", " << name << ") move
        // constructed." << std::endl;
    }

    // 拷贝赋值运算符
    MyStruct &operator=(const MyStruct &other) {
        if (this != &other) {
            id = other.id;
            name = other.name;
        }
        // std::cout << "MyStruct(" << id << ", " << name << ") copy assigned."
        // << std::endl;
        return *this;
    }

    // 移动赋值运算符
    MyStruct &operator=(MyStruct &&other) noexcept {
        if (this != &other) {
            id = other.id;
            name = std::move(other.name);
            other.id = -1; // 标记源对象已移动
            other.name = "MOVED_FROM";
            other.moved_from = true;
        }
        // std::cout << "MyStruct(" << id << ", " << name << ") move assigned."
        // << std::endl;
        return *this;
    }

    // 比较运算符
    bool operator==(const MyStruct &other) const {
        return id == other.id && name == other.name;
    }

    // 析构函数
    ~MyStruct() {
        // std::cout << "MyStruct(" << id << ", " << name << ") destructed." <<
        // std::endl;
    }
};

// --- 具体测试函数 ---

void test_default_construction() {
    TEST_CASE("Default Construction")
    Marcus::any a;
    assert(!a.has_value());
    assert(a.type() == typeid(void));
    TEST_PASSED()
}

void test_value_construction() {
    TEST_CASE("Value Construction")
    Marcus::any a = 123;
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 123);

    Marcus::any b = std::string("hello");
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "hello");

    MyStruct ms(1, "test_struct");
    Marcus::any c = ms; // 拷贝构造
    assert(c.has_value());
    assert(c.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(c) == ms);

    Marcus::any d = MyStruct(2, "temp_struct"); // 移动构造
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(2, "temp_struct"));
    TEST_PASSED()
}

void test_copy_construction() {
    TEST_CASE("Copy Construction")
    Marcus::any original = 456;
    Marcus::any copied = original;
    assert(copied.has_value());
    assert(copied.type() == typeid(int));
    assert(Marcus::any_cast<int>(copied) == 456);
    assert(Marcus::any_cast<int>(original) == 456); // 原始对象应未改变

    Marcus::any original_str = std::string("copy_me");
    Marcus::any copied_str = original_str;
    assert(copied_str.has_value());
    assert(copied_str.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(copied_str) == "copy_me");
    assert(Marcus::any_cast<std::string>(original_str) == "copy_me");
    TEST_PASSED()
}

void test_move_construction() {
    TEST_CASE("Move Construction")
    Marcus::any original = 789;
    Marcus::any moved = std::move(original);
    assert(moved.has_value());
    assert(moved.type() == typeid(int));
    assert(Marcus::any_cast<int>(moved) == 789);
    assert(!original.has_value()); // 原始对象应为空

    Marcus::any original_str = std::string("move_me");
    Marcus::any moved_str = std::move(original_str);
    assert(moved_str.has_value());
    assert(moved_str.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(moved_str) == "move_me");
    assert(!original_str.has_value()); // 原始对象应为空

    MyStruct ms_val(3, "move_struct_original");
    Marcus::any original_ms = ms_val; // 拷贝 MyStruct 到 any
    Marcus::any moved_ms = std::move(original_ms);
    assert(moved_ms.has_value());
    assert(moved_ms.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(moved_ms) ==
           ms_val);                   // 移动后的对象应与原始值相等
    assert(!original_ms.has_value()); // 原始 any 应为空
    TEST_PASSED()
}

void test_in_place_construction() {
    TEST_CASE("In-Place Construction")
    Marcus::any a(Marcus::in_place_type<int>, 100);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    Marcus::any b(Marcus::in_place_type<std::string>, 5, 'x'); // "xxxxx"
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "xxxxx");

    Marcus::any c(Marcus::in_place_type<std::vector<int>>, {1, 2, 3});
    assert(c.has_value());
    assert(c.type() == typeid(std::vector<int>));
    std::vector<int> expected_vec = {1, 2, 3};
    assert(Marcus::any_cast<std::vector<int>>(c) == expected_vec);

    Marcus::any d(Marcus::in_place_type<MyStruct>, 4, "in_place_struct");
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(4, "in_place_struct"));
    TEST_PASSED()
}

void test_copy_assignment() {
    TEST_CASE("Copy Assignment")
    Marcus::any a = 10;
    Marcus::any b;
    b = a; // 拷贝赋值
    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);
    assert(Marcus::any_cast<int>(a) == 10); // 原始对象 unchanged

    Marcus::any c = std::string("original");
    Marcus::any d = 20.0; // 不同的类型
    d = c;                // 拷贝赋值 string 到 double
    assert(d.has_value());
    assert(d.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(d) == "original");
    assert(Marcus::any_cast<std::string>(c) ==
           "original"); // 原始对象 unchanged

    // 自赋值
    Marcus::any e = 50;
    e = e;
    assert(e.has_value());
    assert(e.type() == typeid(int));
    assert(Marcus::any_cast<int>(e) == 50);
    TEST_PASSED()
}

void test_move_assignment() {
    TEST_CASE("Move Assignment")
    Marcus::any a = 10;
    Marcus::any b;
    b = std::move(a); // 移动赋值
    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);
    assert(!a.has_value()); // 原始对象应为空

    Marcus::any c = std::string("original_move");
    Marcus::any d = 20.0; // 不同的类型
    d = std::move(c);     // 移动赋值 string 到 double
    assert(d.has_value());
    assert(d.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(d) == "original_move");
    assert(!c.has_value()); // 原始对象应为空

    // 自移动赋值 (应安全，尽管不常见)
    Marcus::any e = 50;
    e = std::move(e);
    assert(e.has_value()); // 仍应有值
    assert(e.type() == typeid(int));
    assert(Marcus::any_cast<int>(e) == 50);
    TEST_PASSED()
}

void test_value_assignment() {
    TEST_CASE("Value Assignment")
    Marcus::any a;
    a = 100;
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    a = std::string("new_value"); // 赋值不同类型
    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "new_value");

    MyStruct ms(5, "assigned_struct");
    a = ms; // 拷贝赋值 MyStruct
    assert(a.has_value());
    assert(a.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(a) == ms);

    a = MyStruct(6, "temp_struct"); // 移动赋值 rvalue MyStruct
    assert(a.has_value());
    assert(a.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(a) == MyStruct(6, "temp_struct"));
    TEST_PASSED()
}

void test_emplace() {
    TEST_CASE("Emplace")
    Marcus::any a;
    a.emplace<int>(10);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 10);

    a.emplace<std::string>(3, 'a'); // "aaa"
    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "aaa");

    a.emplace<std::vector<double>>({1.1, 2.2, 3.3});
    assert(a.has_value());
    assert(a.type() == typeid(std::vector<double>));
    std::vector<double> expected_vec = {1.1, 2.2, 3.3};
    assert(Marcus::any_cast<std::vector<double>>(a) == expected_vec);

    // 在现有值上 emplace
    Marcus::any b = 123;
    b.emplace<MyStruct>(7, "emplaced_struct");
    assert(b.has_value());
    assert(b.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(b) == MyStruct(7, "emplaced_struct"));
    TEST_PASSED()
}

void test_reset() {
    TEST_CASE("Reset")
    Marcus::any a = 100;
    assert(a.has_value());
    a.reset();
    assert(!a.has_value());
    assert(a.type() == typeid(void));

    Marcus::any b;
    b.reset(); // 重置一个空的 any
    assert(!b.has_value());
    assert(b.type() == typeid(void));
    TEST_PASSED()
}

void test_swap() {
    TEST_CASE("Swap")
    Marcus::any a = 10;
    Marcus::any b = std::string("hello");

    a.swap(b);

    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "hello");

    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);

    Marcus::any c; // 空
    Marcus::any d = 3.14;

    c.swap(d);

    assert(c.has_value());
    assert(c.type() == typeid(double));
    assert(Marcus::any_cast<double>(c) == 3.14);

    assert(!d.has_value());
    assert(d.type() == typeid(void));
    TEST_PASSED()
}

void test_has_value_and_type() {
    TEST_CASE("has_value() and type()")
    Marcus::any a;
    assert(!a.has_value());
    assert(a.type() == typeid(void));

    Marcus::any b = 10;
    assert(b.has_value());
    assert(b.type() == typeid(int));

    Marcus::any c = std::string("test");
    assert(c.has_value());
    assert(c.type() == typeid(std::string));

    c.reset();
    assert(!c.has_value());
    assert(c.type() == typeid(void));
    TEST_PASSED()
}

void test_any_cast_ref() {
    TEST_CASE("any_cast (by reference)")
    Marcus::any a = 10;
    assert(Marcus::any_cast<int>(a) == 10);
    assert(Marcus::any_cast<const int>(a) == 10); // 测试 const 引用

    const Marcus::any ca = std::string("const_str");
    assert(Marcus::any_cast<std::string>(ca) == "const_str");
    assert(Marcus::any_cast<const std::string>(ca) == "const_str");

    Marcus::any b = MyStruct(8, "cast_ref_struct");
    MyStruct &ms_ref = Marcus::any_cast<MyStruct &>(b);
    ms_ref.id = 9; // 修改内部值
    assert(Marcus::any_cast<MyStruct>(b).id == 9);

    // 测试 any_cast 与 rvalue
    Marcus::any c = MyStruct(10, "rvalue_cast_struct");
    MyStruct moved_ms = Marcus::any_cast<MyStruct>(std::move(c)); // 移动出值
    assert(moved_ms == MyStruct(10, "rvalue_cast_struct"));
    // 原始 any 'c' 仍应 has_value()，但其内部对象已处于移动状态
    assert(c.has_value());
    assert(Marcus::any_cast<MyStruct &>(c)
               .moved_from); // 获取引用来检查内部对象的 moved_from 状态

    // 负面测试：类型不匹配
    try {
        Marcus::any_cast<double>(a); // a 包含 int
        TEST_FAILED("Expected BadAnyCast for wrong type.")
    } catch (const Marcus::BadAnyCast &e) {
        std::cout << "  Caught expected exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        TEST_FAILED(std::string("Caught unexpected exception: ") + e.what())
    }

    // 负面测试：空 any
    Marcus::any empty_any;
    try {
        Marcus::any_cast<int>(empty_any); // 空 any
        TEST_FAILED("Expected BadAnyCast for empty any.")
    } catch (const Marcus::BadAnyCast &e) {
        std::cout << "  Caught expected exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        TEST_FAILED(std::string("Caught unexpected exception: ") + e.what())
    }
    TEST_PASSED()
}

void test_any_cast_ptr() {
    TEST_CASE("any_cast (by pointer)")
    Marcus::any a = 10;
    int *p_int = Marcus::any_cast<int>(&a);
    assert(p_int != nullptr);
    assert(*p_int == 10);
    *p_int = 20; // 通过指针修改内部值
    assert(Marcus::any_cast<int>(a) == 20);

    const Marcus::any ca = std::string("const_ptr_str");
    const std::string *p_const_str = Marcus::any_cast<std::string>(&ca);
    assert(p_const_str != nullptr);
    assert(*p_const_str == "const_ptr_str");

    // 负面测试：类型不匹配
    double *p_double = Marcus::any_cast<double>(&a); // a 包含 int
    assert(p_double == nullptr);

    // 负面测试：空 any
    Marcus::any empty_any;
    int *p_empty = Marcus::any_cast<int>(&empty_any);
    assert(p_empty == nullptr);

    const int *p_const_empty = Marcus::any_cast<int>(&empty_any);
    assert(p_const_empty == nullptr);

    // 负面测试：传入 nullptr 给 any_cast(any* operand)
    int *p_null_any =
        Marcus::any_cast<int>(static_cast<Marcus::any *>(nullptr));
    assert(p_null_any == nullptr);
    const int *p_const_null_any =
        Marcus::any_cast<int>(static_cast<const Marcus::any *>(nullptr));
    assert(p_const_null_any == nullptr);
    TEST_PASSED()
}

void test_make_any() {
    TEST_CASE("make_any")
    Marcus::any a = Marcus::make_any<int>(100);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    Marcus::any b = Marcus::make_any<std::string>(5, 'y'); // "yyyyy"
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "yyyyy");

    Marcus::any c = Marcus::make_any<std::vector<int>>({10, 20, 30});
    assert(c.has_value());
    assert(c.type() == typeid(std::vector<int>));
    std::vector<int> expected_vec = {10, 20, 30};
    assert(Marcus::any_cast<std::vector<int>>(c) == expected_vec);

    Marcus::any d = Marcus::make_any<MyStruct>(11, "made_struct");
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(11, "made_struct"));
    TEST_PASSED()
}

int main() {
    std::cout << "Starting Marcus::any tests..." << std::endl << std::endl;

    test_default_construction();
    test_value_construction();
    test_copy_construction();
    test_move_construction();
    test_in_place_construction();
    test_copy_assignment();
    test_move_assignment();
    test_value_assignment();
    test_emplace();
    test_reset();
    test_swap();
    test_has_value_and_type();
    test_any_cast_ref();
    test_any_cast_ptr();
    test_make_any();

    std::cout << "All Marcus::any tests passed successfully!" << std::endl;

    return 0;
}

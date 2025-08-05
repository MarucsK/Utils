#include <iostream>
#include <memory/shared_ptr.hpp>
#include <string>

struct MyObject {
    int id;

    MyObject(int i) : id(i) {
        std::cout << "MyObject " << id << " constructed.\n";
    }

    ~MyObject() {
        std::cout << "MyObject " << id << " destructed.\n";
    }

    void greet() {
        std::cout << "Hello from MyObject " << id << "!\n";
    }
};

// 辅助函数，用于观察 weak_ptr 的状态
void observe(Marcus::weak_ptr<MyObject> wp, const std::string &tag) {
    std::cout << "--- " << tag << " ---\n";
    std::cout << "Weak count: " << wp.use_count()
              << "\n"; // weak_ptr 不直接影响 use_count，但它能查询
    if (wp.expired()) {
        std::cout << "Weak pointer expired. Object already destructed.\n";
    } else {
        Marcus::shared_ptr<MyObject> sp = wp.lock(); // 尝试获取 shared_ptr
        if (sp) {
            std::cout << "Object is alive. Shared count: " << sp.use_count()
                      << "\n";
            sp->greet();
        } else {
            // 理论上如果 !expired() 那么 lock() 应该成功，除非多线程竞争
            std::cout << "Lock failed unexpectedly (object might be in "
                         "destruction phase).\n";
        }
    }
    std::cout << "-------------------\n";
}

int main() {
    std::cout << "Test 1: Basic weak_ptr usage\n";
    Marcus::shared_ptr<MyObject> sp1 = Marcus::make_shared<MyObject>(1);
    Marcus::weak_ptr<MyObject> wp1 = sp1; // 从 shared_ptr 构造 weak_ptr

    observe(wp1, "wp1 after sp1 creation");

    {
        Marcus::shared_ptr<MyObject> sp2 =
            wp1.lock(); // 尝试锁定 weak_ptr，获取 shared_ptr
        if (sp2) {
            std::cout << "Locked sp2. Shared count: " << sp2.use_count()
                      << "\n";
            sp2->greet();
        }
        Marcus::weak_ptr<MyObject> wp2 = wp1; // 从另一个 weak_ptr 复制构造
        observe(wp2, "wp2 copied from wp1");
    } // sp2 在这里超出作用域，MyObject 1 的共享引用计数减少

    observe(wp1, "wp1 after sp2 scope (sp1 still alive)");

    std::cout << "\nTest 2: Object destruction and weak_ptr expiration\n";
    Marcus::shared_ptr<MyObject> sp3; // 声明一个 shared_ptr，稍后赋值
    Marcus::weak_ptr<MyObject> wp3;   // 声明一个 weak_ptr，稍后赋值
    {
        Marcus::shared_ptr<MyObject> temp_sp = Marcus::make_shared<MyObject>(3);
        sp3 = temp_sp; // sp3 现在拥有对象 3 的所有权
        wp3 = temp_sp; // wp3 现在观察对象 3
        std::cout << "Inside temp_sp scope. sp3 shared count: "
                  << sp3.use_count() << "\n";
        observe(wp3, "wp3 inside temp_sp scope");
    } // temp_sp 在这里超出作用域，对象 3 的共享引用计数减少，但 sp3 仍然持有

    std::cout << "After temp_sp scope. sp3 shared count: " << sp3.use_count()
              << "\n";
    observe(wp3, "wp3 after temp_sp scope (sp3 still alive)");

    sp3.reset(); // sp3 释放对象 3 的所有权。由于 sp3 是最后一个
                 // shared_ptr，对象 3 在这里被析构。
    observe(wp3, "wp3 after sp3 reset (object 3 should be expired)");

    std::cout << "\nTest 3: reset() and swap()\n";
    Marcus::shared_ptr<MyObject> sp4 = Marcus::make_shared<MyObject>(4);
    Marcus::weak_ptr<MyObject> wp4 = sp4;
    Marcus::weak_ptr<MyObject> wp5; // 默认构造，空 weak_ptr

    observe(wp4, "wp4 initial");
    observe(wp5, "wp5 initial (empty)");

    wp5 = wp4; // 赋值操作
    observe(wp5, "wp5 after assignment from wp4");

    wp4.reset(); // 重置 wp4，使其不再观察任何对象
    observe(wp4, "wp4 after reset");
    observe(wp5, "wp5 after wp4 reset (should still be valid as it's a copy)");

    Marcus::shared_ptr<MyObject> sp5 = Marcus::make_shared<MyObject>(5);
    Marcus::weak_ptr<MyObject> wp6 = sp5;
    Marcus::weak_ptr<MyObject> wp7 = wp5; // wp7 观察对象 4 (因为wp5观察对象4)

    std::cout << "Before swap: wp6 observes "
              << (wp6.expired() ? "expired" : std::to_string(wp6.lock()->id))
              << ", wp7 observes "
              << (wp7.expired() ? "expired" : std::to_string(wp7.lock()->id))
              << "\n";
    wp6.swap(wp7); // 交换 wp6 和 wp7 的内容
    std::cout << "After swap: wp6 observes "
              << (wp6.expired() ? "expired" : std::to_string(wp6.lock()->id))
              << ", wp7 observes "
              << (wp7.expired() ? "expired" : std::to_string(wp7.lock()->id))
              << "\n";

    // 显式销毁 shared_ptr，触发对象析构
    sp1.reset(); // 触发 MyObject 1 析构
    sp4.reset(); // 触发 MyObject 4 析构
    sp5.reset(); // 触发 MyObject 5 析构

    std::cout << "\nTest 4: Final observation of weak_ptr states\n";
    observe(wp1, "wp1 at end of main (object 1 should be expired)");
    observe(wp5, "wp5 at end of main (object 4 should be expired)");
    observe(
        wp6,
        "wp6 at end of main (object 4 should be expired)"); // wp6 现在观察对象
                                                            // 4
    observe(
        wp7,
        "wp7 at end of main (object 5 should be expired)"); // wp7 现在观察对象
                                                            // 5

    return 0;
}

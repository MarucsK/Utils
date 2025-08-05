#include <cstdio>
#include <iostream>
#include <memory/unique_ptr.hpp>
#include <vector>

struct MyClass {
    int a, b, c;
};

struct Animal {
    virtual void speak() = 0;
    virtual ~Animal() = default;
};

struct Dog : Animal {
    int age;

    Dog(int age_) : age(age_) {}

    virtual void speak() {
        printf("Bark! I'm %d Year Old!\n", age);
    }
};

struct Cat : Animal {
    int &age;

    Cat(int &age_) : age(age_) {}

    virtual void speak() {
        printf("Meow! I'm %d Year Old!\n", age);
    }
};

int main() {
    std::vector<Marcus::unique_ptr<Animal>> zoo;
    int age = 3;
    zoo.push_back(Marcus::make_unique<Cat>(age));
    zoo.push_back(Marcus::make_unique<Dog>(age));
    for (const auto &a: zoo) {
        a->speak();
    }
    age++;
    for (const auto &a: zoo) {
        a->speak();
    }

    auto p = Marcus::make_unique<int[]>(5);
    // auto p2 = Marcus::make_unique<int[5]>();
    auto p2 = Marcus::make_unique_for_overwrite<int[]>(5);
    return 0;
}

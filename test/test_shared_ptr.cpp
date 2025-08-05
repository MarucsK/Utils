#include <iostream>
#include <memory/shared_ptr.hpp>

struct Student : Marcus::enable_shared_from_this<Student> {
    const char *name;
    int age;

    explicit Student(const char *name_, int age_) : name(name_), age(age_) {
        std::cout << "Student构造" << std::endl;
    }

    Student(Student &&) = delete;

    void func() {
        std::cout << (void *)shared_from_this().get() << '\n';
    }

    ~Student() {
        std::cout << "Student 析构" << std::endl;
    }
};

struct StudentDerived : Student {
    explicit StudentDerived(const char *name_, int age_)
        : Student(name_, age_) {
        std::cout << "StudentDerived 构造" << std::endl;
    }

    ~StudentDerived() {
        std::cout << "StudentDerived 析构" << std::endl;
    }
};

int main() {
    Marcus::shared_ptr<Student> p0(new StudentDerived("Marcus", 23));
    auto dp = Marcus::static_pointer_cast<StudentDerived>(p0);
    Marcus::shared_ptr<const Student> bp = p0;
    p0 = Marcus::const_pointer_cast<Student>(bp);
    std::cout << dp->name << '\n';

    Marcus::shared_ptr<Student> p = Marcus::make_shared<Student>("Marcus", 23);
    Marcus::shared_ptr<Student> p2(new Student("Marcus", 23));
    Marcus::shared_ptr<Student> p3(new Student("Marcus", 23),
                                   [](Student *p) { delete p; });
    Student *raw_p = p.get();
    std::cout << raw_p->name << raw_p->age << std::endl;
    Marcus::shared_ptr<Student> p4 = p;
    Marcus::shared_ptr<Student> p5 = p3;

    p5->func();

    p3 = p5;
    std::cout << p->name << ' ' << p->age << std::endl;
    std::cout << raw_p->name << ' ' << raw_p->age << std::endl;
}

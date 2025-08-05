#include <iostream>
#include <type_traits/type_traits.hpp>
#include <type_traits>

using namespace Marcus;

template <typename T, typename U>
void test_is_same(bool expected) {
    std::cout << "is_same<" << typeid(T).name() << ", " << typeid(U).name()
              << ">: " << (is_same_v<T, U> == expected ? "PASS" : "FAIL")
              << std::endl;
}

void test_constexpr_bool(const char *name, bool value, bool expected) {
    std::cout << name << ": " << (value == expected ? "PASS" : "FAIL")
              << std::endl;
}

void type_traits_test() {
    // integral_constant, true_type, false_type
    test_constexpr_bool("integral_constant",
                        integral_constant<int, 3>::value == 3, true);
    test_constexpr_bool("true_type", true_type::value, true);
    test_constexpr_bool("false_type", false_type::value, false);

    // type_identity
    test_is_same<type_identity_t<int>, int>(true);

    // is_void
    test_constexpr_bool("is_void<void>", is_void_v<void>, true);
    test_constexpr_bool("is_void<int>", is_void_v<int>, false);

    // is_null_pointer
    test_constexpr_bool("is_null_pointer<std::nullptr_t>",
                        is_null_pointer_v<std::nullptr_t>, true);
    test_constexpr_bool("is_null_pointer<int>", is_null_pointer_v<int>, false);

    // remove_const, remove_volatile, remove_cv
    test_is_same<remove_const_t<const int>, int>(true);
    test_is_same<remove_volatile_t<volatile int>, int>(true);
    test_is_same<remove_cv_t<const volatile int>, int>(true);

    // is_integral
    test_constexpr_bool("is_integral<int>", is_integral_v<int>, true);
    test_constexpr_bool("is_integral<float>", is_integral_v<float>, false);

    // is_floating_point
    test_constexpr_bool("is_floating_point<float>", is_floating_point_v<float>,
                        true);
    test_constexpr_bool("is_floating_point<int>", is_floating_point_v<int>,
                        false);

    // is_array
    test_constexpr_bool("is_array<int[]>", is_array_v<int[]>, true);
    test_constexpr_bool("is_array<int[3]>", is_array_v<int[3]>, true);
    test_constexpr_bool("is_array<int>", is_array_v<int>, false);

    // is_pointer
    test_constexpr_bool("is_pointer<int*>", is_pointer_v<int *>, true);
    test_constexpr_bool("is_pointer<int>", is_pointer_v<int>, false);

    // is_lvalue_reference, is_rvalue_reference, is_reference
    test_constexpr_bool("is_lvalue_reference<int&>",
                        is_lvalue_reference_v<int &>, true);
    test_constexpr_bool("is_rvalue_reference<int&&>",
                        is_rvalue_reference_v<int &&>, true);
    test_constexpr_bool("is_reference<int&>", is_reference_v<int &>, true);
    test_constexpr_bool("is_reference<int>", is_reference_v<int>, false);

    // remove_reference
    test_is_same<remove_reference_t<int &>, int>(true);
    test_is_same<remove_reference_t<int &&>, int>(true);

    // is_function
    test_constexpr_bool("is_function<void()>", is_function_v<void()>, true);
    test_constexpr_bool("is_function<int>", is_function_v<int>, false);

    // is_member_pointer, is_member_function_pointer, is_member_object_pointer
    struct S {
        int x;

        void f() {}
    };

    test_constexpr_bool("is_member_pointer<int S::*>",
                        is_member_pointer_v<int S::*>, true);
    test_constexpr_bool("is_member_function_pointer<void (S::*)()>",
                        is_member_function_pointer_v<void (S::*)()>, true);
    test_constexpr_bool("is_member_object_pointer<int S::*>",
                        is_member_object_pointer_v<int S::*>, true);

    // is_arithmetic, is_fundamental, is_scalar, is_object, is_compound
    test_constexpr_bool("is_arithmetic<int>", is_arithmetic_v<int>, true);
    test_constexpr_bool("is_fundamental<int>", is_fundamental_v<int>, true);
    test_constexpr_bool("is_scalar<int>", is_scalar_v<int>, true);
    test_constexpr_bool("is_object<int>", is_object_v<int>, true);
    test_constexpr_bool("is_compound<int>", is_compound_v<int>, false);
    test_constexpr_bool("is_compound<S>", is_compound_v<S>, true);

    // is_const, is_volatile
    test_constexpr_bool("is_const<const int>", is_const_v<const int>, true);
    test_constexpr_bool("is_volatile<volatile int>",
                        is_volatile_v<volatile int>, true);

    // add_const, add_volatile, add_pointer, remove_pointer
    test_is_same<add_const_t<int>, const int>(true);
    test_is_same<add_volatile_t<int>, volatile int>(true);
    test_is_same<add_pointer_t<int>, int *>(true);
    test_is_same<remove_pointer_t<int *>, int>(true);

    // add_lvalue_reference, add_rvalue_reference
    test_is_same<add_lvalue_reference_t<int>, int &>(true);
    test_is_same<add_rvalue_reference_t<int>, int &&>(true);

    // remove_extent, remove_all_extents
    test_is_same<remove_extent_t<int[3]>, int>(true);
    test_is_same<remove_all_extents_t<int[3][4]>, int>(true);

    // conditional
    test_is_same<conditional_t<true, int, float>, int>(true);
    test_is_same<conditional_t<false, int, float>, float>(true);

    // decay
    test_is_same<decay_t<int &>, int>(true);
    test_is_same<decay_t<int[3]>, int *>(true);
    test_is_same<decay_t<void(int)>, void (*)(int)>(true);

    // conjunction, disjunction, negation
    test_constexpr_bool("conjunction<true_type, true_type>",
                        conjunction_v<true_type, true_type>, true);
    test_constexpr_bool("disjunction<false_type, true_type>",
                        disjunction_v<false_type, true_type>, true);
    test_constexpr_bool("negation<true_type>", negation_v<true_type>, false);

    // enable_if
    std::cout << "enable_if<true, int>::type is int: "
              << (std::is_same<enable_if_t<true, int>, int>::value ? "PASS"
                                                                   : "FAIL")
              << std::endl;
}

int main() {
    type_traits_test();
    return 0;
}

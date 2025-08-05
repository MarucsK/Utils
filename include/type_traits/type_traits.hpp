#pragma once

#include <cstddef>

namespace Marcus {

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;

    constexpr operator value_type() const noexcept {
        return value;
    }

    constexpr value_type operator()() const noexcept {
        return value;
    }
};

using true_type = integral_constant<bool, true>;

using false_type = integral_constant<bool, false>;

// type_identity
template <typename T>
struct type_identity {
    using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

// is_void
template <typename>
struct is_void : false_type {};

template <>
struct is_void<void> : true_type {};

template <typename T>
inline constexpr bool is_void_v = is_void<T>::value;

// is_null_pointer
template <typename T>
struct is_null_pointer : false_type {};

template <>
struct is_null_pointer<std::nullptr_t> : true_type {};

template <>
struct is_null_pointer<const std::nullptr_t> : true_type {};

template <>
struct is_null_pointer<volatile std::nullptr_t> : true_type {};

template <>
struct is_null_pointer<const volatile std::nullptr_t> : true_type {};

template <typename T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

// remove const
template <typename T>
struct remove_const {
    using type = T;
};

template <typename T>
struct remove_const<const T> {
    using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

// remove_volatile
template <typename T>
struct remove_volatile {
    using type = T;
};

template <typename T>
struct remove_volatile<volatile T> {
    using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

// remove_cv
template <typename T>
struct remove_cv {
    using type = typename remove_volatile<typename remove_const<T>::type>::type;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

// is_integral
template <typename T>
struct is_integral_helper : false_type {};

template <>
struct is_integral_helper<bool> : true_type {};

template <>
struct is_integral_helper<char> : true_type {};

template <>
struct is_integral_helper<signed char> : true_type {};

template <>
struct is_integral_helper<unsigned char> : true_type {};

template <>
struct is_integral_helper<wchar_t> : true_type {};

#ifdef __cpp_char8_t
template <>
struct is_integral_helper<char8_t> : true_type {};
#endif

template <>
struct is_integral_helper<char16_t> : true_type {};

template <>
struct is_integral_helper<char32_t> : true_type {};

template <>
struct is_integral_helper<short> : true_type {};

template <>
struct is_integral_helper<unsigned short> : true_type {};

template <>
struct is_integral_helper<int> : true_type {};

template <>
struct is_integral_helper<unsigned int> : true_type {};

template <>
struct is_integral_helper<long> : true_type {};

template <>
struct is_integral_helper<unsigned long> : true_type {};

template <>
struct is_integral_helper<long long> : true_type {};

template <>
struct is_integral_helper<unsigned long long> : true_type {};

template <typename T>
struct is_integral : is_integral_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

// is_floating_point
template <typename>
struct is_floating_point_helper : false_type {};

template <>
struct is_floating_point_helper<float> : true_type {};

template <>
struct is_floating_point_helper<double> : true_type {};

template <>
struct is_floating_point_helper<long double> : true_type {};

template <typename T>
struct is_floating_point : is_floating_point_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

// is_array
template <typename T>
struct is_array : false_type {};

template <typename T>
struct is_array<T[]> : true_type {};

template <typename T, std::size_t N>
struct is_array<T[N]> : true_type {};

template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;

// is_pointer
template <typename T>
struct is_pointer_helper : false_type {};

template <typename T>
struct is_pointer_helper<T *> : true_type {};

template <typename T>
struct is_pointer : is_pointer_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

// is_lvalue_reference is_rvalue_reference
template <typename T>
struct is_lvalue_reference : false_type {};

template <typename T>
struct is_lvalue_reference<T &> : true_type {};

template <typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename T>
struct is_rvalue_reference : false_type {};

template <typename T>
struct is_rvalue_reference<T &&> : true_type {};

template <typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T>
struct is_reference : integral_constant<bool, is_lvalue_reference_v<T> ||
                                                  is_rvalue_reference_v<T>> {};

template <typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

// remove_reference
template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T &> {
    using type = T;
};

template <typename T>
struct remove_reference<T &&> {
    using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

// is_function
template <typename>
struct is_function : public false_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...)> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......)> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) volatile> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) volatile> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes...) const volatile> : public true_type {};

template <typename _Res, typename... _ArgTypes>
struct is_function<_Res(_ArgTypes......) const volatile> : public true_type {};

template <typename T>
inline constexpr bool is_function_v = is_function<T>::value;

// is_member_pointer
template <typename T>
struct is_member_pointer_helper : false_type {};

template <typename T, typename C>
struct is_member_pointer_helper<T C::*> : true_type {};

template <typename T>
struct is_member_pointer : is_member_pointer_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

// is_member_function_pointer
template <typename T>
struct is_member_function_pointer_helper : false_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...)> : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) volatile>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const volatile>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) &> : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const &>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) volatile &>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const volatile &>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) &&> : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const &&>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) volatile &&>
    : true_type {};

template <typename Ret, typename C, typename... Args>
struct is_member_function_pointer_helper<Ret (C::*)(Args...) const volatile &&>
    : true_type {};

template <typename T>
struct is_member_function_pointer
    : is_member_function_pointer_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_member_function_pointer_v =
    is_member_function_pointer<T>::value;

// is_member_object_pointer
template <typename T>
struct is_member_object_pointer
    : integral_constant<bool, is_member_pointer_v<T> &&
                                  !is_member_function_pointer_v<T>> {};

template <typename T>
inline constexpr bool is_member_object_pointer_v =
    is_member_object_pointer<T>::value;

// is_arithmetic
template <typename T>
struct is_arithmetic
    : integral_constant<bool, is_integral_v<T> || is_floating_point_v<T>> {};

template <typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

// is_fundamental
template <typename T>
struct is_fundamental
    : integral_constant<bool, is_arithmetic_v<T> || is_void_v<T> ||
                                  is_null_pointer_v<T>> {};

template <typename T>
inline constexpr bool is_fundamental_v = is_fundamental<T>::value;

// is_scalar
template <typename T>
struct is_scalar
    : integral_constant<bool, is_arithmetic_v<T> || is_pointer_v<T> ||
                                  is_member_pointer_v<T> ||
                                  is_null_pointer_v<T>> {};

template <typename T>
inline constexpr bool is_scalar_v = is_scalar<T>::value;

// is_object
template <typename T>
struct is_object
    : integral_constant<bool, !is_reference_v<T> && !is_void_v<T>> {};

template <typename T>
inline constexpr bool is_object_v = is_object<T>::value;

// is_compund
template <typename T>
struct is_compound : integral_constant<bool, !is_fundamental_v<T>> {};

template <typename T>
inline constexpr bool is_compound_v = is_compound<T>::value;

// is_const
template <typename T>
struct is_const : false_type {};

template <typename T>
struct is_const<const T> : true_type {};

template <typename T>
inline constexpr bool is_const_v = is_const<T>::value;

// is_volatile
template <typename T>
struct is_volatile : false_type {};

template <typename T>
struct is_volatile<volatile T> : true_type {};

template <typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;

// is_same
template <typename T, typename U>
struct is_same : false_type {};

template <typename T>
struct is_same<T, T> : true_type {};

template <typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

// add_const
template <typename T>
struct add_const {
    using type = const T;
};

template <typename T>
struct add_const<T &> {
    using type = T &;
};

template <typename T>
struct add_const<T &&> {
    using type = T &&;
};

template <typename T>
using add_const_t = typename add_const<T>::type;

// add_volatile
template <typename T>
struct add_volatile {
    using type = volatile T;
};

template <typename T>
struct add_volatile<T &> {
    using type = T &;
};

template <typename T>
struct add_volatile<T &&> {
    using type = T &&;
};

template <typename T>
using add_volatile_t = typename add_volatile<T>::type;

// add_pointer
template <typename T>
struct add_pointer_helper {
    using type = remove_reference_t<T> *;
};

template <typename T>
struct add_pointer {
    using type = typename add_pointer_helper<T>::type;
};

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

// remove_pointer
template <typename T>
struct remove_pointer {
    using type = T;
};

template <typename T>
struct remove_pointer<T *> {
    using type = T;
};

template <typename T>
struct remove_pointer<T *const> {
    using type = T;
};

template <typename T>
struct remove_pointer<T *volatile> {
    using type = T;
};

template <typename T>
struct remove_pointer<T *const volatile> {
    using type = T;
};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

// add_lvalue_reference add_rvalue_reference
template <typename T>
auto try_add_lvalue_reference(int) -> type_identity<T &>;

template <typename T>
auto try_add_lvalue_reference(...) -> type_identity<T>;

template <typename T>
auto try_add_rvalue_reference(int) -> type_identity<T &&>;

template <typename T>
auto try_add_rvalue_reference(...) -> type_identity<T>;

template <typename T>
struct add_lvalue_reference : decltype(try_add_lvalue_reference<T>(0)) {};

template <typename T>
struct add_rvalue_reference : decltype(try_add_rvalue_reference<T>(0)) {};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

// remove_extent
template <typename T>
struct remove_extent {
    using type = T;
};

template <typename T>
struct remove_extent<T[]> {
    using type = T;
};

template <typename T, std::size_t N>
struct remove_extent<T[N]> {
    using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

// remove_all_extents
template <typename T>
struct remove_all_extents {
    using type = T;
};

template <typename T>
struct remove_all_extents<T[]> {
    using type = typename remove_all_extents<T>::type;
};

template <typename T, std::size_t N>
struct remove_all_extents<T[N]> {
    using type = typename remove_all_extents<T>::type;
};

template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

// conditional
template <bool B, typename T, typename F>
struct conditional {
    using type = T;
};

template <typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

// decay
template <typename T>
struct decay {
private:
    using U = remove_reference_t<T>;

public:
    using type = conditional_t<
        is_array_v<U>, add_pointer_t<remove_extent_t<U>>,
        conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>>;
};

template <typename T>
using decay_t = typename decay<T>::type;

// conjunction
template <typename... B>
struct conjunction;

template <>
struct conjunction<> : true_type {};

template <typename B1>
struct conjunction<B1> : B1 {};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...>
    : conditional_t<B1::value, conjunction<Bn...>, B1> {};

template <typename... B>
inline constexpr bool conjunction_v = conjunction<B...>::value;

// disjunction
template <typename... B>
struct disjunction;

template <>
struct disjunction<> : false_type {};

template <typename B1>
struct disjunction<B1> : B1 {};

template <typename B1, typename... Bn>
struct disjunction<B1, Bn...>
    : conditional_t<B1::value, B1, disjunction<Bn...>> {};

template <typename... B>
inline constexpr bool disjunction_v = disjunction<B...>::value;

template <typename B>
struct negation : integral_constant<bool, !B::value> {};

template <typename B>
inline constexpr bool negation_v = negation<B>::value;

// enable_if
template <bool B, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

} // namespace Marcus

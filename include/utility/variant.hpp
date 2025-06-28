#pragma once

#include <algorithm>
#include <type_traits>
#include <functional>

namespace Marcus {

template <size_t I>
struct In_place_index {
    explicit In_place_index() = default;
};

template <size_t I>
constexpr In_place_index<I> in_place_index;

struct BadVariantAccess : std::exception {
    BadVariantAccess() = default;
    virtual ~BadVariantAccess() = default;

    const char *what() const noexcept override { return "BadVariantAccess"; }
};

template <typename, typename>
struct variant_index;

template <typename, size_t>
struct variant_alternative;

template <typename... _Ts>
struct variant {
  private:
    size_t _index;

    alignas(
        std::max({alignof(_Ts)...})) char _union[std::max({sizeof(_Ts)...})];

    using DestructorFunction = void (*)(char *) noexcept;

    // 返回一个函数指针数组，每个元素对应一个备选类型的析构函数
    static DestructorFunction *destructors_table() noexcept {
        static DestructorFunction function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_p) noexcept {
                reinterpret_cast<_Ts *>(_union_p)->~_Ts();
            }...};
        return function_ptrs;
    }

    using CopyConstructorFunction = void (*)(char *, const char *) noexcept;

    // 返回一个函数指针数组，每个元素对应一个备选类型的拷贝构造函数
    static CopyConstructorFunction *copy_constructors_table() noexcept {
        static CopyConstructorFunction function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_dst, const char *_union_src) noexcept {
                new (_union_dst)
                    _Ts(*reinterpret_cast<const _Ts *>(_union_src));
            }...};
        return function_ptrs;
    }

    using CopyAssignmentFunction = void (*)(char *, const char *) noexcept;

    static CopyAssignmentFunction *copy_assigment_functions_table() noexcept {
        static CopyAssignmentFunction function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_dst, const char *_union_src) noexcept {
                *reinterpret_cast<_Ts *>(_union_dst) =
                    *reinterpret_cast<const _Ts *>(_union_src);
            }...};
        return function_ptrs;
    }

    using MoveConstructorFunction = void (*)(char *, char *) noexcept;

    static MoveConstructorFunction *move_constructors_table() noexcept {
        static MoveConstructorFunction function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_dst, char *_union_src) noexcept {
                new (_union_dst)
                    _Ts(std::move(*reinterpret_cast<const _Ts *>(_union_src)));
            }...};
        return function_ptrs;
    }

    using MoveAssignmentFunction = void (*)(char *, char *) noexcept;

    static MoveAssignmentFunction *move_assigment_functions_table() noexcept {
        static MoveAssignmentFunction function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_dst, char *_union_src) noexcept {
                *reinterpret_cast<_Ts *>(_union_dst) =
                    std::move(*reinterpret_cast<_Ts *>(_union_src));
            }...};
        return function_ptrs;
    }

    template <typename Lambda>
    using ConstVisitorFunction = std::common_type<
        typename std::invoke_result<Lambda, const _Ts &>::type...>::
        type (*)(const char *, Lambda &&);

    // 返回一个函数指针数组，每个元素用于对 const Variant 调用 visit
    template <typename Lambda>
    static ConstVisitorFunction<Lambda> *const_visitors_table() noexcept {
        static ConstVisitorFunction<Lambda> function_ptrs[sizeof...(_Ts)] = {
            [](const char *_union_p, Lambda &&lambda) ->
            typename std::invoke_result<Lambda, const _Ts &>::type {
                std::invoke(
                    std::forward<Lambda>(lambda),
                    *reinterpret_cast<const _Ts *>(_union_p));
            }...};
        return function_ptrs;
    }

    template <typename Lambda>
    using VisitorFunction =
        std::common_type<typename std::invoke_result<Lambda, _Ts &>::type...>::
            type (*)(char *, Lambda &&);

    // 返回一个函数指针数组，每个元素用于对 非const Variant 调用 visit
    template <typename Lambda>
    static VisitorFunction<Lambda> *visitors_table() noexcept {
        static VisitorFunction<Lambda> function_ptrs[sizeof...(_Ts)] = {
            [](char *_union_p, Lambda &&lambda)
                -> std::common_type<
                    typename std::invoke_result<Lambda, _Ts &>::type...>::type {
                return std::invoke(
                    std::forward<Lambda>(lambda),
                    *reinterpret_cast<_Ts *>(_union_p));
            }...};
        return function_ptrs;
    }

  public:
    template <
        typename _T,
        typename std::enable_if<
            std::disjunction<std::is_same<_T, _Ts>...>::value,
            int>::type = 0>
    variant(_T __value) : _index(variant_index<variant, _T>::value) {
        _T *__p = reinterpret_cast<_T *>(_union);
        new (__p) _T(__value);
    }

    variant(const variant &__other) : _index(__other._index) {
        copy_constructors_table()[index()](_union, __other._union);
    }

    variant &operator=(const variant &__other) {
        _index = __other._index;
        copy_assigment_functions_table()[index()](_union, __other._union);
    }

    variant(variant &&__other) : _index(__other._index) {
        move_constructors_table()[index()](_union, __other._union);
    }

    variant &operator=(variant &&__other) {
        _index = __other._index;
        move_assigment_functions_table()[index()](_union, __other._union);
    }

    template <size_t I, typename... Args>
    explicit variant(In_place_index<I>, Args &&...args) : _index(I) {
        new (_union) typename variant_alternative<variant, I>::type(
            std::forward<Args>(args)...);
    }

    ~variant() noexcept { destructors_table()[index()](_union); }

    template <typename Lambda>
    std::common_type<typename std::invoke_result<Lambda, _Ts &>::type...>::type
    visit(Lambda &&lambda) {
        return visitors_table<Lambda>()[index()](
            _union, std::forward<Lambda>(lambda));
    }

    template <typename Lambda>
    std::common_type<
        typename std::invoke_result<Lambda, const _Ts &>::type...>::type
    visit(Lambda &&lambda) const {
        return const_visitors_table<Lambda>()[index()](
            _union, std::forward<Lambda>(lambda));
    }

    constexpr size_t index() const noexcept { return _index; }

    template <typename T>
    constexpr bool holds_alternative() const noexcept {
        return variant_index<variant, T>::value == index();
    }

    template <size_t I>
    typename variant_alternative<variant, I>::type &get() {
        static_assert(I < sizeof...(_Ts), "I out of range!");
        if ( _index != I ) { throw BadVariantAccess(); }
        return *reinterpret_cast<
            typename variant_alternative<variant, I>::type *>(_union);
    }

    template <typename T>
    T &get() {
        return get<variant_index<variant, T>::value>();
    }

    template <size_t I>
    const typename variant_alternative<variant, I>::type &get() const {
        static_assert(I < sizeof...(_Ts), "I out of range!");
        if ( _index != I ) { throw BadVariantAccess(); }
        return *reinterpret_cast<
            const typename variant_alternative<variant, I>::type *>(_union);
    }

    template <typename T>
    const T &get() const {
        return get<variant_index<variant, T>::value>();
    }

    template <size_t I>
    typename variant_alternative<variant, I>::type *get_if() {
        static_assert(I < sizeof...(_Ts), "I out of range");
        if ( _index != I ) { return nullptr; }
        return reinterpret_cast<
            typename variant_alternative<variant, I>::type *>(_union);
    }

    template <typename T>
    T *get_if() {
        return get_if<variant_index<variant, T>::value>();
    }

    template <size_t I>
    const typename variant_alternative<variant, I>::type *get_if() const {
        static_assert(I < sizeof...(_Ts), "I out of range");
        if ( _index != I ) { return nullptr; }
        return reinterpret_cast<
            const typename variant_alternative<variant, I>::type *>(_union);
    }

    template <typename T>
    const T *get_if() const {
        return get_if<variant_index<variant, T>::value>();
    }
};

template <typename T, typename... Ts>
struct variant_alternative<variant<T, Ts...>, 0> {
    using type = T;
};

template <typename T, typename... Ts, size_t I>
struct variant_alternative<variant<T, Ts...>, I> {
    using type = typename variant_alternative<variant<Ts...>, I - 1>::type;
};

template <typename T, typename... Ts>
struct variant_index<variant<T, Ts...>, T> {
    static constexpr size_t value = 0;
};

template <typename T0, typename T, typename... Ts>
struct variant_index<variant<T0, Ts...>, T> {
    static constexpr size_t value = variant_index<variant<Ts...>, T>::value + 1;
};

} // namespace Marcus
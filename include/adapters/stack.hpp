#pragma once

#include <containers/deque.hpp>

namespace Marcus {

template <typename _Tp, typename _Container = deque<_Tp>>
class stack {
public:
    using container_type = _Container;
    using value_type = typename _Container::value_type;
    using size_type = typename _Container::size_type;
    using reference = typename _Container::reference;
    using const_reference = typename _Container::const_reference;

protected:
    _Container c;

public:
    stack() = default;

    explicit stack(const _Container &__cont) : c(__cont) {}

    explicit stack(_Container &&__cont) noexcept(
        std::is_nothrow_move_constructible_v<_Container>)
        : c(std::move(__cont)) {}

    stack(const stack &__other) : c(__other.c) {}

    stack(stack &&__other) noexcept(
        std::is_nothrow_move_constructible_v<_Container>)
        : c(std::move(__other.c)) {}

    stack &operator=(const stack &__other) {
        c = __other.c;
        return *this;
    }

    stack &operator=(stack &&__other) noexcept(
        std::is_nothrow_move_assignable_v<_Container>) {
        c = std::move(__other.c);
        return *this;
    }

    template <typename _Alloc, typename = std::enable_if_t<std::uses_allocator<
                                   container_type, _Alloc>::value>>
    explicit stack(const _Alloc &__alloc) : c(__alloc) {}

    template <typename _Alloc, typename = std::enable_if_t<std::uses_allocator<
                                   container_type, _Alloc>::value>>
    stack(const container_type &__cont, const _Alloc &__alloc)
        : c(__cont, __alloc) {}

    template <typename _Alloc, typename = std::enable_if_t<std::uses_allocator<
                                   container_type, _Alloc>::value>>
    stack(container_type &&__cont, const _Alloc &__alloc)
        : c(std::move(__cont), __alloc) {}

    template <typename _Alloc, typename = std::enable_if_t<std::uses_allocator<
                                   container_type, _Alloc>::value>>
    stack(const stack &__other, const _Alloc &__alloc)
        : c(__other.c, __alloc) {}

    template <typename _Alloc, typename = std::enable_if_t<std::uses_allocator<
                                   container_type, _Alloc>::value>>
    stack(stack &&__other, const _Alloc &__alloc)
        : c(std::move(__other.c), __alloc) {}

    reference top() {
        return c.back();
    }

    const_reference top() const {
        return c.back();
    }

    [[nodiscard]] bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    void push(const value_type &__val) {
        c.push_back(__val);
    }

    void push(value_type &&__val) {
        c.push_back(std::move(__val));
    }

    template <typename... _Args>
    reference emplace(_Args &&...__args) {
        return c.emplace_back(std::forward<_Args>(__args)...);
    }

    void pop() {
        c.pop_back();
    }

    void
    swap(stack &__other) noexcept(std::is_nothrow_swappable_v<_Container>) {
        using std::swap;
        swap(c, __other.c);
    }

    template <typename _T, typename _C>
    friend bool operator==(const stack<_T, _C> &, const stack<_T, _C> &);

#if __cpp_lib_three_way_comparison
    template <typename _T, typename _C>
    friend std::strong_ordering operator<=>(const stack<_T, _C> &,
                                            const stack<_T, _C> &);
#else
    template <typename _T, typename _C>
    friend bool operator!=(const stack<_T, _C> &, const stack<_T, _C> &);
    template <typename _T, typename _C>
    friend bool operator<(const stack<_T, _C> &, const stack<_T, _C> &);
    template <typename _T, typename _C>
    friend bool operator>(const stack<_T, _C> &, const stack<_T, _C> &);
    template <typename _T, typename _C>
    friend bool operator<=(const stack<_T, _C> &, const stack<_T, _C> &);
    template <typename _T, typename _C>
    friend bool operator>=(const stack<_T, _C> &, const stack<_T, _C> &);
#endif
};

template <typename _Tp, typename _Container>
inline bool operator==(const stack<_Tp, _Container> &__lhs,
                       const stack<_Tp, _Container> &__rhs) {
    return __lhs.c == __rhs.c;
}

#if __cpp_lib_three_way_comparison
template <typename _Tp, typename _Container>
inline std::strong_ordering operator<=>(const stack<_Tp, _Container> &__lhs,
                                        const stack<_Tp, _Container> &__rhs) {
    return __lhs.c <=> __rhs.c;
}
#else
template <typename _Tp, typename _Container>
inline bool operator!=(const stack<_Tp, _Container> &__lhs,
                       const stack<_Tp, _Container> &__rhs) {
    return __lhs.c != __rhs.c;
}

template <typename _Tp, typename _Container>
inline bool operator<(const stack<_Tp, _Container> &__lhs,
                      const stack<_Tp, _Container> &__rhs) {
    return __lhs.c < __rhs.c;
}

template <typename _Tp, typename _Container>
inline bool operator>(const stack<_Tp, _Container> &__lhs,
                      const stack<_Tp, _Container> &__rhs) {
    return __lhs.c > __rhs.c;
}

template <typename _Tp, typename _Container>
inline bool operator<=(const stack<_Tp, _Container> &__lhs,
                       const stack<_Tp, _Container> &__rhs) {
    return __lhs.c <= __rhs.c;
}

template <typename _Tp, typename _Container>
inline bool operator>=(const stack<_Tp, _Container> &__lhs,
                       const stack<_Tp, _Container> &__rhs) {
    return __lhs.c >= __rhs.c;
}
#endif

template <typename _Tp, typename _Container>
void swap(stack<_Tp, _Container> &__lhs,
          stack<_Tp, _Container> &__rhs) noexcept(noexcept(__lhs.swap(__rhs))) {
    __lhs.swap(__rhs);
}

} // namespace Marcus

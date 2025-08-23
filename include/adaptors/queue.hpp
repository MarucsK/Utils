#include <containers/deque.hpp>

namespace Marcus {

template <typename _Tp, typename _Container = deque<_Tp>>
class queue {
public:
    using container_type = _Container;
    using value_type = typename container_type::value_type;
    using size_type = typename container_type::size_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

protected:
    container_type c;

public:
    queue() : c() {}

    explicit queue(const container_type &__cont) : c(__cont) {}

    explicit queue(container_type &&__cont) : c(std::move(__cont)) {}

    queue(const queue &__other) : c(__other.c) {}

    queue(queue &&__other) noexcept(
        std::is_nothrow_move_constructible_v<container_type>)
        : c(std::move(__other.c)) {}

    template <class Alloc, typename = std::enable_if_t<
                               std::uses_allocator_v<container_type, Alloc>>>
    explicit queue(const Alloc &__alloc) : c(__alloc) {}

    template <class Alloc, typename = std::enable_if_t<
                               std::uses_allocator_v<container_type, Alloc>>>
    queue(const container_type &__cont, const Alloc &__alloc)
        : c(__cont, __alloc) {}

    template <class Alloc, typename = std::enable_if_t<
                               std::uses_allocator_v<container_type, Alloc>>>
    queue(container_type &&__cont, const Alloc &__alloc)
        : c(std::move(__cont), __alloc) {}

    template <class Alloc, typename = std::enable_if_t<
                               std::uses_allocator_v<container_type, Alloc>>>
    queue(const queue &__other, const Alloc &__alloc) : c(__other.c, __alloc) {}

    template <class Alloc, typename = std::enable_if_t<
                               std::uses_allocator_v<container_type, Alloc>>>
    queue(queue &&__other, const Alloc &__alloc)
        : c(std::move(__other.c), __alloc) {}

    ~queue() = default;

    queue &operator=(const queue &__other) {
        c = __other.c;
        return *this;
    }

    queue &operator=(queue &&__other) noexcept(
        std::is_nothrow_move_assignable_v<container_type>) {
        c = std::move(__other.c);
        return *this;
    }

    reference front() {
        return c.front();
    }

    const_reference front() const {
        return c.front();
    }

    reference back() {
        return c.back();
    }

    const_reference back() const {
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
        c.pop_front();
    }

    void
    swap(queue &__other) noexcept(std::is_nothrow_swappable_v<container_type>) {
        using std::swap;
        swap(c, __other.c);
    }

#if __cpp_lib_three_way_comparison
    template <class _T, class _C>
    friend std::compare_three_way_result_t<_C>
    operator<=>(const queue<_T, _C> &__lhs, const queue<_T, _C> &__rhs);
#else
    template <class _T, class _C>
    friend bool operator==(const queue<_T, _C> &__lhs,
                           const queue<_T, _C> &__rhs);
    template <class _T, class _C>
    friend bool operator!=(const queue<_T, _C> &__lhs,
                           const queue<_T, _C> &__rhs);
    template <class _T, class _C>
    friend bool operator<(const queue<_T, _C> &__lhs,
                          const queue<_T, _C> &__rhs);
    template <class _T, class _C>
    friend bool operator<=(const queue<_T, _C> &__lhs,
                           const queue<_T, _C> &__rhs);
    template <class _T, class _C>
    friend bool operator>(const queue<_T, _C> &__lhs,
                          const queue<_T, _C> &__rhs);
    template <class _T, class _C>
    friend bool operator>=(const queue<_T, _C> &__lhs,
                           const queue<_T, _C> &__rhs);
#endif
};

template <class _Tp, class _Container>
inline bool operator==(const queue<_Tp, _Container> &__lhs,
                       const queue<_Tp, _Container> &__rhs) {
    return __lhs.c == __rhs.c;
}

#if __cpp_lib_three_way_comparison
template <class _Tp, class _Container>
inline std::compare_three_way_result_t<_Container>
operator<=>(const queue<_Tp, _Container> &__lhs,
            const queue<_Tp, _Container> &__rhs) {
    return __lhs.c <=> __rhs.c;
}
#else
template <class _Tp, class _Container>
inline bool operator!=(const queue<_Tp, _Container> &__lhs,
                       const queue<_Tp, _Container> &__rhs) {
    return !(__lhs == __rhs);
}

template <class _Tp, class _Container>
inline bool operator<(const queue<_Tp, _Container> &__lhs,
                      const queue<_Tp, _Container> &__rhs) {
    return __lhs.c < __rhs.c;
}

template <class _Tp, class _Container>
inline bool operator<=(const queue<_Tp, _Container> &__lhs,
                       const queue<_Tp, _Container> &__rhs) {
    return !(__rhs < __lhs);
}

template <class _Tp, class _Container>
inline bool operator>(const queue<_Tp, _Container> &__lhs,
                      const queue<_Tp, _Container> &__rhs) {
    return __rhs < __lhs;
}

template <class _Tp, class _Container>
inline bool operator>=(const queue<_Tp, _Container> &__lhs,
                       const queue<_Tp, _Container> &__rhs) {
    return !(__lhs < __rhs);
}
#endif

template <typename _Tp, typename _Container>
void swap(queue<_Tp, _Container> &__lhs,
          queue<_Tp, _Container> &__rhs) noexcept(noexcept(__lhs.swap(__rhs))) {
    __lhs.swap(__rhs);
}

} // namespace Marcus

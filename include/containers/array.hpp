#pragma once

#include <algorithm> // std::equal
#include <core/_common.hpp>
#include <cstddef>   // size_t
#include <iterator>  // std::reverse_iterator
#include <stdexcept> // std::out_of_range

namespace Marcus {

template <typename _Tp, size_t _N>
struct array {
    using value_type = _Tp;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = _Tp *;
    using const_pointer = const _Tp *;
    using reference = _Tp &;
    using const_reference = const _Tp &;
    using iterator = _Tp *;
    using const_iterator = const _Tp *;
    using reverse_iterator = std::reverse_iterator<_Tp *>;
    using const_reverse_iterator = std::reverse_iterator<const _Tp *>;

    _Tp _M_elements[_N];

    _Tp &operator[](size_t __i) noexcept {
        return _M_elements[__i];
    }

    const _Tp &operator[](size_t __i) const noexcept {
        return _M_elements[__i];
    }

    _Tp &at(size_t __i) {
        if (__i >= _N) [[unlikely]] {
            throw std::out_of_range("array:at");
        }
        return _M_elements[__i];
    }

    const _Tp &at(size_t __i) const {
        if (__i >= _N) [[unlikely]] {
            throw std::out_of_range("array:at");
        }
        return _M_elements[__i];
    }

    void
    fill(const _Tp &__val) noexcept(std::is_nothrow_copy_assignable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; __i++) {
            _M_elements[__i] = __val;
        }
    }

    void swap(array &__other) noexcept(std::is_nothrow_swappable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; __i++) {
            std::swap(_M_elements[__i], __other._M_elements[__i]);
        }
    }

    _Tp &front() noexcept {
        return _M_elements[0];
    }

    const _Tp &front() const noexcept {
        return _M_elements[0];
    }

    _Tp &back() noexcept {
        return _M_elements[_N - 1];
    }

    const _Tp &back() const noexcept {
        return _M_elements[_N - 1];
    }

    static constexpr bool empty() noexcept {
        return false;
    }

    static constexpr size_t size() noexcept {
        return _N;
    }

    static constexpr size_t max_size() noexcept {
        return _N;
    }

    const _Tp *data() const noexcept {
        return _M_elements;
    }

    const _Tp *cdata() const noexcept {
        return _M_elements;
    }

    _Tp *data() noexcept {
        return _M_elements;
    }

    const _Tp *cbegin() const noexcept {
        return _M_elements;
    }

    const _Tp *cend() const noexcept {
        return _M_elements + _N;
    }

    const _Tp *begin() const noexcept {
        return _M_elements;
    }

    const _Tp *end() const noexcept {
        return _M_elements + _N;
    }

    _Tp *begin() noexcept {
        return _M_elements;
    }

    _Tp *end() noexcept {
        return _M_elements + _N;
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    _LIBPENGCXX_DEFINE_COMPARISON(array); // 自动生成比较运算符
};

template <typename _Tp>
struct array<_Tp, 0> {
    using value_type = _Tp;
    using pointer = _Tp *;
    using const_pointer = const _Tp *;
    using reference = _Tp &;
    using const_reference = const _Tp &;
    using iterator = _Tp *;
    using const_iterator = const _Tp *;
    using reverse_iterator = std::reverse_iterator<_Tp *>;
    using const_reverse_iterator = std::reverse_iterator<const _Tp *>;

    _Tp &operator[](size_t __i) noexcept {
        _LIBPENGCXX_UNREACHABLE(); // 编译期优化，永远不会到达此位置
    }

    _Tp &operator[](size_t __i) const noexcept {
        _LIBPENGCXX_UNREACHABLE();
    }

    _Tp &at(size_t __i) {
        throw std::out_of_range("array::at");
    }

    const _Tp &at(size_t __i) const {
        throw std::out_of_range("array::at");
    }

    void fill(const _Tp &) noexcept {}

    void swap(array &) noexcept {}

    _Tp &front() noexcept {
        _LIBPENGCXX_UNREACHABLE();
    }

    const _Tp &front() const noexcept {
        _LIBPENGCXX_UNREACHABLE();
    }

    _Tp &back() noexcept {
        _LIBPENGCXX_UNREACHABLE();
    }

    const _Tp &back() const noexcept {
        _LIBPENGCXX_UNREACHABLE();
    }

    static constexpr bool empty() noexcept {
        return true;
    }

    static constexpr size_t size() noexcept {
        return 0;
    }

    static constexpr size_t max_size() noexcept {
        return 0;
    }

    const _Tp *cdata() const noexcept {
        return nullptr;
    }

    const _Tp *data() const noexcept {
        return nullptr;
    }

    _Tp *data() noexcept {
        return nullptr;
    }

    const _Tp *cbegin() const noexcept {
        return nullptr;
    }

    const _Tp *cend() const noexcept {
        return nullptr;
    }

    const _Tp *begin() const noexcept {
        return nullptr;
    }

    const _Tp *end() const noexcept {
        return nullptr;
    }

    _Tp *begin() noexcept {
        return nullptr;
    }

    _Tp *end() noexcept {
        return nullptr;
    }

    const _Tp *crbegin() const noexcept {
        return nullptr;
    }

    const _Tp *crend() const noexcept {
        return nullptr;
    }

    const _Tp *rbegin() const noexcept {
        return nullptr;
    }

    const _Tp *rend() const noexcept {
        return nullptr;
    }

    _Tp *rbegin() noexcept {
        return nullptr;
    }

    _Tp *rend() noexcept {
        return nullptr;
    }

    _LIBPENGCXX_DEFINE_COMPARISON(array);
};

// Derivation succeeds only if all constructor parameters are of the same type.
template <typename _Tp, typename... _Ts>
array(_Tp, _Ts...)
    -> array<std::enable_if_t<(std::is_same_v<_Tp, _Ts> && ...), _Tp>,
             1 + sizeof...(_Ts)>;

} // namespace Marcus

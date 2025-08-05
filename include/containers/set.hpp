#pragma once

#include <containers/core/_RbTree.hpp>
#include <core/_common.hpp>

namespace Marcus {

template <typename _Tp, typename _Compare = std::less<_Tp>,
          typename _Alloc = std::allocator<_Tp>>
struct set : _RbTreeImpl<const _Tp, _Compare, _Alloc> {
    using typename _RbTreeImpl<const _Tp, _Compare, _Alloc>::const_iterator;
    using typename _RbTreeImpl<const _Tp, _Compare, _Alloc>::node_type;
    using iterator = const_iterator;
    using value_type = _Tp;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    set() = default;

    explicit set(_Compare __comp)
        : _RbTreeImpl<const _Tp, _Compare, _Alloc>(__comp) {}

    set(set &&) = default;

    set &operator=(set &&) = default;

    set(const set &__other) : _RbTreeImpl<const _Tp, _Compare, _Alloc>() {
        this->_M_single_insert(__other.begin(), __other.end());
    }

    set &operator=(const set &__other) {
        if (&__other != this) {
            this->assign(__other.begin(), __other.end());
        }
        return *this;
    }

    set &operator=(std::initializer_list<_Tp> __ilist) {
        this->assign(__ilist);
        return *this;
    }

    void assign(std::initializer_list<_Tp> __ilist) {
        this->clear();
        this->_M_single_insert(__ilist.begin(), __ilist.end());
    }

    _Compare value_comp() const noexcept {
        return this->_M_comp;
    }

    template <typename _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    const_iterator find(_Tp &&__value) const noexcept {
        return this->_M_find(__value);
    }

    const_iterator find(const _Tp &__value) const noexcept {
        return this->_M_find(__value);
    }

    std::pair<iterator, bool> insert(_Tp &&__value) {
        return this->_M_single_emplace(std::move(__value));
    }

    std::pair<iterator, bool> insert(const _Tp &__value) {
        return this->_M_single_emplace(__value);
    }

    template <typename... _Ts>
    std::pair<iterator, bool> emplace(_Ts &&...__value) {
        return this->_M_single_emplace(std::forward<_Ts>(__value)...);
    }

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator,
                                                     _InputIt)>
    void insert(_InputIt __first, _InputIt __last) {
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<const _Tp, _Compare, _Alloc>::assign;

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator,
                                                     _InputIt)>
    void assign(_InputIt __first, _InputIt __last) {
        this->clear();
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<const _Tp, _Compare, _Alloc>::erase;

    template <typename _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    std::size_t erase(_Tv &&__value) {
        return this->_M_single_erase(__value);
    }

    std::size_t erase(const _Tp &__value) {
        return this->_M_single_erase(__value);
    }

    template <typename _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    std::size_t count(_Tv &&__value) const noexcept {
        return this->_M_contains(__value) ? 1 : 0;
    }

    std::size_t count(const _Tp &__value) const noexcept {
        return this->_M_contains(__value) ? 1 : 0;
    }

    template <typename _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    bool contains(_Tv &&__value) const noexcept {
        return this->_M_contains(__value);
    }

    bool contains(const _Tp &__value) const noexcept {
        return this->_M_contains(__value);
    }

    template <typename _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    node_type extract(_Tv &&__value) {
        iterator __it = this->_M_find(__value);
        return __it != this->end() ? this->extract(__it) : node_type();
    }

    node_type extract(const _Tp &__value) {
        iterator __it = this->_M_find(__value);
        return __it != this->end() ? this->extract(__it) : node_type();
    }
};

template <typename _Tp, typename _Compare = std::less<_Tp>,
          typename _Alloc = std::allocator<_Tp>>
struct multiset : _RbTreeImpl<const _Tp, _Compare, _Alloc> {
    using typename _RbTreeImpl<const _Tp, _Compare, _Alloc>::const_iterator;
    using typename _RbTreeImpl<const _Tp, _Compare, _Alloc>::node_type;
    using iterator = const_iterator;
    using value_type = _Tp;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    multiset() = default;

    explicit multiset(_Compare __comp)
        : _RbTreeImpl<const _Tp, _Compare, _Alloc>(__comp) {}

    multiset(multiset &&) = default;

    multiset &operator=(multiset &&) = default;

    multiset(const multiset &__other)
        : _RbTreeImpl<const _Tp, _Compare, _Alloc>() {
        this->_M_multi_insert(__other.begin(), __other.end());
    }

    multiset &operator=(const multiset &__other) {
        if (&__other != this) {
            this->assign(__other.begin(), __other.end());
        }
        return *this;
    }

    multiset &operator=(std::initializer_list<_Tp> __ilist) {
        this->assign(__ilist);
        return *this;
    }

    void assign(std::initializer_list<_Tp> __ilist) {
        this->clear();
        this->_M_multi_insert(__ilist.begin(), __ilist.end());
    }

    _Compare value_comp() const noexcept {
        return this->_M_comp;
    }

    template <class _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    const_iterator find(_Tp &&__value) const noexcept {
        return this->_M_find(__value);
    }

    const_iterator find(const _Tp &__value) const noexcept {
        return this->_M_find(__value);
    }

    iterator insert(_Tp &&__value) {
        return this->_M_multi_emplace(std::move(__value));
    }

    iterator insert(const _Tp &__value) {
        return this->_M_multi_emplace(__value);
    }

    template <typename... _Ts>
    iterator emplace(_Ts &&...__value) {
        return this->_M_multi_emplace(std::forward<_Ts>(__value)...);
    }

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator,
                                                     _InputIt)>
    void insert(_InputIt __first, _InputIt __last) {
        return this->_M_multi_insert(__first, __last);
    }

    using _RbTreeImpl<const _Tp, _Compare, _Alloc>::assign;

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator,
                                                     _InputIt)>
    void assign(_InputIt __first, _InputIt __last) {
        this->clear();
        return this->_M_multi_insert(__first, __last);
    }

    using _RbTreeImpl<const _Tp, _Compare, _Alloc>::erase;

    template <class _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    std::size_t erase(_Tv &&__value) {
        return this->_M_multi_erase(__value);
    }

    std::size_t erase(const _Tp &__value) {
        return this->_M_multi_erase(__value);
    }

    template <class _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    std::size_t count(_Tv &&__value) const noexcept {
        return this->_M_multi_count(__value);
    }

    std::size_t count(const _Tp &__value) const noexcept {
        return this->_M_multi_count(__value);
    }

    template <class _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    bool contains(_Tv &&__value) const noexcept {
        return this->_M_contains(__value);
    }

    bool contains(const _Tp &__value) const noexcept {
        return this->_M_contains(__value);
    }

    template <class _Tv,
              _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_Compare, _Tv, _Tp)>
    node_type extract(_Tv &&__value) {
        iterator __it = this->_M_find(__value);
        return __it != this->end() ? this->extract(__it) : node_type();
    }

    node_type extract(const _Tp &__value) {
        iterator __it = this->_M_find(__value);
        return __it != this->end() ? this->extract(__it) : node_type();
    }
};

} // namespace Marcus

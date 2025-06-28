#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>

#include <core/_common.hpp>
#include <containers/core/_RbTree.hpp>

namespace Marcus {

// 将原始的键比较器_Compare 适配到 比较键值对
template <typename _Compare, typename _Value, typename = void>
struct _RbTreeValueCompare {
  protected:
    [[no_unique_address]] _Compare _M_comp;

  public:
    _RbTreeValueCompare(_Compare __comp = _Compare()) noexcept
        : _M_comp(__comp) {}

    // 比较 键和键值对
    bool operator()(
        typename _Value::first_type const &__lhs,
        _Value const &__rhs) const noexcept {
        return this->_M_comp(__lhs, __rhs.first);
    }

    // 比较 键值对和键
    bool operator()(
        _Value const &__lhs,
        typename _Value::first_type const &__rhs) const noexcept {
        return this->_M_comp(__lhs.first, __rhs);
    }

    // 比较 两个键值对
    bool operator()(_Value const &__lhs, _Value const &__rhs) const noexcept {
        return this->_M_comp(__lhs.first, __rhs.first);
    }

    struct _RBTreeIsMap;
};

template <typename _Compare, typename _Value>
struct _RbTreeValueCompare<
    _Compare,
    _Value,
    decltype((void)static_cast<typename _Compare::is_transparent *>(
        nullptr))> { // 当 _Compare 有 is_transparent 成员时启用
    [[no_unique_address]] _Compare _M_comp;

    _RbTreeValueCompare(_Compare __comp = _Compare()) noexcept
        : _M_comp(__comp) {}

    template <typename _Lhs>
    bool operator()(_Lhs &&__lhs, _Value const &__rhs) const noexcept {
        return this->_M_comp(__lhs, __rhs.first);
    }

    template <typename _Rhs>
    bool operator()(_Value const &__lhs, _Rhs &&__rhs) const noexcept {
        return this->_M_comp(__lhs.first, __rhs);
    }

    bool operator()(_Value const &__lhs, _Value const &__rhs) const noexcept {
        return this->_M_comp(__lhs.first, __rhs.first);
    }

    using is_transparent = typename _Compare::is_transparent;
};

template <
    typename _Key,
    typename _Mapped,
    typename _Compare = std::less<_Key>,
    typename _Alloc = std::allocator<std::pair<const _Key, _Mapped>>>
struct map
    : _RbTreeImpl<
          std::pair<const _Key, _Mapped>,
          _RbTreeValueCompare<_Compare, std::pair<const _Key, _Mapped>>,
          _Alloc> {
    using key_type = _Key;
    using mapped_type = _Mapped;
    using value_type = std::pair<_Key const, _Mapped>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    using _ValueComp = _RbTreeValueCompare<_Compare, value_type>;

  public:
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::iterator;
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::const_iterator;
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::node_type;

    map() = default;

    explicit map(_Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {}

    map(std::initializer_list<value_type> __ilist) {
        _M_single_insert(__ilist.begin(), __ilist.end());
    }

    explicit map(std::initializer_list<value_type> __ilist, _Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {
        _M_single_insert(__ilist.begin(), __ilist.end());
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    explicit map(_InputIt __first, _InputIt __last) {
        _M_single_insert(__first, __last);
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    explicit map(_InputIt __first, _InputIt __last, _Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {
        _M_single_insert(__first, __last);
    }

    map(map &&) = default;

    map &operator=(map &&) = default;

    map(const map &__other) : _RbTreeImpl<value_type, _ValueComp, _Alloc>() {
        this->_M_single_insert(__other.begin(), __other.end());
    }

    map &operator=(const map &__other) {
        if ( &__other != this ) {
            this->assign(__other.begin(), __other.end());
        }
        return *this;
    }

    map &operator=(std::initializer_list<value_type> __ilist) {
        this->assign(__ilist);
        return *this;
    }

    void assign(std::initializer_list<value_type> __ilist) {
        this->clear();
        this->_M_single_insert(__ilist.begin(), __ilist.end());
    }

    _Compare key_comp() const noexcept { return this->_M_comp->_M_comp; }

    _ValueComp value_comp() const noexcept { return this->_M_comp; }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    iterator find(_Kv &&__key) noexcept {
        return this->_M_find(__key);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    const_iterator find(_Kv &&__key) const noexcept {
        return this->_M_find(__key);
    }

    iterator find(const _Key &__key) noexcept { return this->_M_find(__key); }

    const_iterator find(const _Key &__key) const noexcept {
        return this->_M_find(__key);
    }

    std::pair<iterator, bool> insert(value_type &&__value) {
        return this->_M_single_emplace(std::move(__value));
    }

    std::pair<iterator, bool> insert(const value_type &__value) {
        return this->_M_single_emplace(__value);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    const _Mapped &at(const _Kv &__key) const {
        const_iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) [[unlikely]] {
            throw std::out_of_range("map::at");
        }
        return __it->second;
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    _Mapped &at(const _Kv &__key) {
        iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) [[unlikely]] {
            throw std::out_of_range("map::at");
        }
        return __it->second;
    }

    const _Mapped &at(const _Key &__key) const {
        const_iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) [[unlikely]] {
            throw std::out_of_range("map::at");
        }
        return __it->second;
    }

    _Mapped &at(const _Key &__key) {
        iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) [[unlikely]] {
            throw std::out_of_range("map::at");
        }
        return __it->second;
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    _Mapped &operator[](const _Kv &__key) {
        iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) {
            __it =
                this->_M_single_emplace(
                        std::piecewise_construct, std::forward_as_tuple(__key),
                        std::forward_as_tuple())
                    .first;
        }
        return __it->second;
    }

    _Mapped &operator[](const _Key &__key) {
        iterator __it = this->_M_find(__key);
        if ( __it == this->end() ) {
            __it =
                this->_M_single_emplace(
                        std::piecewise_construct, std::forward_as_tuple(__key),
                        std::forward_as_tuple())
                    .first;
        }
        return __it->second;
    }

    template <
        typename _Mp,
        typename = std::enable_if_t<std::is_convertible_v<_Mp, _Mapped>>>
    std::pair<iterator, bool>
    insert_or_assign(const _Key &__key, _Mp &&__mapped) {
        std::pair<iterator, bool> __result = this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(__key),
            std::forward_as_tuple(std::forward<_Mp>(__mapped)));
        if ( !__result.second ) {
            __result.first->second = std::forward<_Mp>(__mapped);
        }
        return __result;
    }

    template <
        typename _Mp,
        typename = std::enable_if_t<std::is_convertible_v<_Mp, _Mapped>>>
    std::pair<iterator, bool> insert_or_assign(_Key &&__key, _Mp &&__mapped) {
        std::pair<iterator, bool> __result = this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(std::move(__key)),
            std::forward_as_tuple(std::forward<_Mp>(__mapped)));
        if ( !__result.second ) {
            __result.first->second = std::forward<_Mp>(__mapped);
        }
        return __result;
    }

    template <typename... Vs>
    std::pair<iterator, bool> emplace(Vs &&...__value) {
        return this->_M_single_emplace(std::forward<Vs>(__value)...);
    }

    template <typename... _Ms>
    std::pair<iterator, bool> try_emplace(_Key &&__key, _Ms &&...__mapped) {
        return this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(std::move(__key)),
            std::forward_as_tuple(std::forward<_Ms>(__mapped)...));
    }

    template <typename... _Ms>
    std::pair<iterator, bool>
    try_emplace(const _Key &__key, _Ms &&...__mapped) {
        return this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(__key),
            std::forward_as_tuple(std::forward<_Ms>(__mapped)...));
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    iterator insert(_InputIt __first, _InputIt __last) {
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<value_type, _ValueComp, _Alloc>::assign;

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    iterator assign(_InputIt __first, _InputIt __last) {
        this->clear();
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<value_type, _ValueComp, _Alloc>::erase;

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    size_t erase(_Kv &&__key) {
        return this->_M_single_erase(__key);
    }

    size_t erase(const _Key &__key) { return this->_M_single_erase(__key); }

    template <
        class _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    size_t count(_Kv &&__key) const noexcept {
        return this->_M_contains(__key) ? 1 : 0;
    }

    size_t count(const _Key &__key) const noexcept {
        return this->_M_contains(__key) ? 1 : 0;
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    bool contains(_Kv &&__key) const noexcept {
        return this->_M_contains(__key);
    }

    bool contains(const _Key &__key) const noexcept {
        return this->_M_contains(__key);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    node_type extract(_Kv &&__key) {
        iterator __it = this->_M_find(__key);
        return __it != this->end() ? this->extract(__it) : node_type();
    }

    node_type extract(const _Key &__key) {
        iterator __it = this->_M_find(__key);
        return __it != this->end() ? this->extract(__it) : node_type();
    }
};

template <
    typename _Key,
    typename _Mapped,
    typename _Compare = std::less<_Key>,
    typename _Alloc = std::allocator<std::pair<const _Key, _Mapped>>>
struct multimap
    : _RbTreeImpl<
          std::pair<const _Key, _Mapped>,
          _RbTreeValueCompare<_Compare, std::pair<const _Key, _Mapped>>,
          _Alloc> {
    using key_type = _Key;
    using mapped_type = _Mapped;
    using value_type = std::pair<_Key const, _Mapped>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    using _ValueComp = _RbTreeValueCompare<_Compare, value_type>;

  public:
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::iterator;
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::const_iterator;
    using typename _RbTreeImpl<value_type, _ValueComp, _Alloc>::node_type;

    multimap() = default;

    explicit multimap(_Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {}

    multimap(std::initializer_list<value_type> __ilist) {
        _M_multi_insert(__ilist.begin(), __ilist.end());
    }

    explicit multimap(
        std::initializer_list<value_type> __ilist, _Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {
        _M_multi_insert(__ilist.begin(), __ilist.end());
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    explicit multimap(_InputIt __first, _InputIt __last) {
        _M_multi_insert(__first, __last);
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    explicit multimap(_InputIt __first, _InputIt __last, _Compare __comp)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>(__comp) {
        _M_multi_insert(__first, __last);
    }

    multimap(multimap &&) = default;

    multimap &operator=(multimap &&) = default;

    multimap(const multimap &__other)
        : _RbTreeImpl<value_type, _ValueComp, _Alloc>() {
        this->_M_multi_insert(__other.begin(), __other.end());
    }

    multimap &operator=(const multimap &__other) {
        if ( &__other != this ) {
            return assign(__other.begin(), __other.end());
        }
        return *this;
    }

    multimap &operator=(std::initializer_list<value_type> __ilist) {
        this->assign(__ilist);
        return *this;
    }

    void assign(std::initializer_list<value_type> __ilist) {
        this->clear();
        this->_M_multi_insert(__ilist.begin(), __ilist.end());
    }

    _Compare key_comp() const noexcept { return this->_M_comp->_M_comp; }

    _ValueComp value_comp() const noexcept { return this->_M_comp; }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    iterator find(_Kv &&__key) noexcept {
        return this->_M_find(__key);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    const iterator find(_Kv &&__key) const noexcept {
        return this->_M_find(__key);
    }

    iterator find(const _Key &__key) noexcept { return this->_M_find(__key); }

    const_iterator find(const _Key &__key) const noexcept {
        return this->_M_find(__key);
    }

    std::pair<iterator, bool> insert(value_type &&__value) {
        return this->_M_single_emplace(std::move(__value));
    }

    std::pair<iterator, bool> insert(const value_type &__value) {
        return this->_M_single_emplace(__value);
    }

    template <typename... _Ts>
    std::pair<iterator, bool> emplace(_Ts &&...__value) {
        return this->_M_single_emplace(std::forward<_Ts>(__value)...);
    }

    template <typename... _Ts>
    std::pair<iterator, bool> try_emplace(_Key &&__key, _Ts &&...__value) {
        return this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(std::move(__key)),
            std::forward_as_tuple(std::forward<_Ts>(__value)...));
    }

    template <typename... _Ts>
    std::pair<iterator, bool> try_emplace(const _Key &__key, _Ts &&...__value) {
        return this->_M_single_emplace(
            std::piecewise_construct, std::forward_as_tuple(__key),
            std::forward_as_tuple(std::forward<_Ts>(__value)...));
    }

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    iterator insert(_InputIt __first, _InputIt __last) {
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<value_type, _ValueComp, _Alloc>::assign;

    template <
        _LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::input_iterator, _InputIt)>
    iterator assign(_InputIt __first, _InputIt __last) {
        this->clear();
        return this->_M_single_insert(__first, __last);
    }

    using _RbTreeImpl<value_type, _ValueComp, _Alloc>::erase;

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    size_t erase(_Kv &&__key) {
        return this->_M_single_erase(__key);
    }

    size_t erase(const _Key &__key) { return this->_M_single_erase(__key); }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    size_t count(_Kv &&__value) const noexcept {
        return this->_M_multi_count(__value);
    }

    size_t count(const _Key &__value) const noexcept {
        return this->_M_multi_count(__value);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    bool contains(_Kv &&__value) const noexcept {
        return this->_M_contains(__value);
    }

    bool contains(const _Key &__key) const noexcept {
        return this->_M_contains(__key);
    }

    template <
        typename _Kv,
        _LIBPENGCXX_REQUIRES_TRANSPARENT_COMPARE(_ValueComp, _Kv, value_type)>
    node_type extract(_Kv &&__key) {
        iterator __it = this->_M_find(__key);
        return __it != this->end() ? this->extract(__it) : node_type();
    }

    node_type extract(const _Key &__key) {
        iterator __it = this->_M_find(__key);
        return __it != this->end() ? this->extract(__it) : node_type();
    }
};

} // namespace Marcus

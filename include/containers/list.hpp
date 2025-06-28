#pragma once

#include <memory>
#include <initializer_list>
#include <limits>

#include <core/_common.hpp>

namespace Marcus {

template <typename T>
struct ListBaseNode {
    ListBaseNode *_next;
    ListBaseNode *_prev;

    inline T &value();
    inline const T &value() const;
};
template <typename T>
struct ListValueNode : ListBaseNode<T> {
    union {
        T _value;
    };
};

template <typename T>
inline T &ListBaseNode<T>::value() {
    return static_cast<ListValueNode<T> &>(*this)._value;
}

template <typename T>
inline const T &ListBaseNode<T>::value() const {
    return static_cast<const ListValueNode<T> &>(*this)._value;
}

template <typename T, typename Alloc = std::allocator<T>>
struct list {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;

  private:
    using ListNode = ListBaseNode<T>;
    /*
    List 存储的是 T 类型的元素，但实际在内存中分配和管理的是 ListValueNode<T>
    类型的节点（包含数据和指针） 因此需要一个能够分配 ListValueNode<T>
    的空间配置器 而不是直接使用用于分配 T 的 Alloc。rebind_alloc
    提供了这种转换能力。 因此，AllocNode 表示一个能够分配 ListValueNode<T>
    类型的空间配置器。
    */
    using AllocNode = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ListValueNode<T>>;

    ListNode _dummy;
    std::size_t _size;
    [[no_unique_address]] Alloc _alloc;

    ListNode *newNode() {
        AllocNode allocNode(_alloc);
        /*
        std::allocator_traits<AllocNode>: 这是 std::allocator_traits
        的特化，用于处理 AllocNode 类型的空间配置器。
        ::allocate(allocNode, 1): 这是调用 std::allocator_traits 的静态成员函数
        allocate。 allocNode: 这是要使用的空间配置器实例。 1: 表示要分配一个
        ListValueNode<T> 对象的内存。
        返回一个指向分配的内存块的指针，这个内存块足够存储一个 ListValueNode<T>
        对象。
        */
        return std::allocator_traits<AllocNode>::allocate(allocNode, 1);
    }

    void deleteNode(ListNode *node) noexcept {
        AllocNode allocNode(_alloc);
        std::allocator_traits<AllocNode>::deallocate(
            allocNode, static_cast<ListValueNode<T> *>(node), 1);
    }

  public:
    list() noexcept {
        _size = 0;
        _dummy._prev = _dummy._next = &_dummy;
    }

    explicit list(const Alloc &alloc) noexcept : _alloc(alloc) {
        _size = 0;
        _dummy._prev = _dummy._next = &_dummy;
    }

    list(list &&other) noexcept : _alloc(std::move(other._alloc)) {
        _uninit_move_assign(std::move(other));
    }

    list(list &&other, const Alloc &alloc) noexcept : _alloc(alloc) {
        _uninit_move_assign(std::move(other));
    }

    list &operator=(list &&other) {
        _alloc = std::move(other._alloc);
        clear();
        _uninit_move_assign(std::move(other));
    }

  private:
    void _uninit_move_assign(list &&other) {
        auto prev = other._dummy._prev;
        auto next = other._dummy._next;
        prev->_next = &_dummy;
        next->_prev = &_dummy;
        _dummy = other._dummy;
        other._dummy._prev = other._dummy._next = &other._dummy;
        _size = other._size;
        other._size = 0;
    }

  public:
    list(const list &other) : _alloc(other._alloc) {
        _uninit_assign(other.cbegin(), other.cend());
    }

    list(const list &other, const Alloc &alloc) : _alloc(alloc) {
        _uninit_assign(other.cbegin(), other.cend());
    }

    list &operator=(const list &other) { assign(other.cbegin(), other.cend()); }

    bool empty() const noexcept { return _dummy._next == &_dummy; }

    T &front() noexcept { return _dummy._next->value(); }

    T &back() noexcept { return _dummy._prev->value(); }

    const T &front() const noexcept { return _dummy._next->value(); }

    const T &back() const noexcept { return _dummy._prev->value(); }

    explicit list(size_t n, const Alloc &alloc = Alloc()) : _alloc(alloc) {
        _uninit_assign(n);
    }

    explicit list(size_t n, const T &val, const Alloc &alloc = Alloc())
        : _alloc(alloc) {
        _uninit_assign(n, val);
    }

    template <std::input_iterator InputIt>
    list(InputIt first, InputIt last, const Alloc &alloc = Alloc())
        : _alloc(alloc) {
        _uninit_assign(first, last);
    }

    list(std::initializer_list<T> _ilist, const Alloc &alloc = Alloc())
        : list(_ilist.begin(), _ilist.end(), alloc) {}

    list &operator=(std::initializer_list<T> _ilist) { assign(_ilist); }

  private:
    template <std::input_iterator InputIt>
    void _uninit_assign(InputIt first, InputIt last) {
        _size = 0;
        ListNode *pre = &_dummy;
        while ( first != last ) {
            ListNode *node = newNode();
            pre->_next = node;
            node->_prev = pre;
            std::construct_at(&node->value(), *first);
            pre = node;
            ++first;
            ++_size;
        }
        _dummy._prev = pre;
        pre->_next = &_dummy;
    }

    template <typename... Args>
        requires std::constructible_from<value_type, Args...>
    void _uninit_assign(size_t n, Args &&...args) {
        ListNode *pre = &_dummy;
        for ( size_t i = 0; i < n; ++i ) {
            ListNode *node = newNode();
            pre->_next = node;
            node->_prev = pre;
            std::construct_at(&node->value(), std::forward<Args>(args)...);
            pre = node;
        }
        _dummy._prev = pre;
        pre->_next = &_dummy;
        _size = n;
    }

  public:
    std::size_t size() const noexcept { return _size; }

    constexpr std::size_t max_size() const noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        _uninit_assign(first, last);
    }

    void assign(std::initializer_list<T> _ilist) {
        clear();
        _uninit_assign(_ilist.begin(), _ilist.end());
    }

    void assign(size_t n, const T &val) {
        clear();
        _uninit_assign(n, val);
    }

    void push_back(const T &val) { emplace_back(val); }

    void push_back(T &&val) { emplace_back(std::move(val)); }

    void push_front(const T &val) { emplace_front(val); }

    void push_front(T &&val) { emplace_front(std::move(val)); }

    template <typename... Args>
    T &emplace_back(Args &&...args) {
        ListNode *node = newNode();
        ListNode *pre = _dummy._prev;
        pre->_next = node;
        node->_prev = pre;
        node->_next = &_dummy;
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        _dummy._prev = node;
        ++_size;
        return node->value();
    }

    template <typename... Args>
    T &emplace_front(Args &&...args) {
        ListNode *node = newNode();
        ListNode *nxt = _dummy._next;
        nxt->_prev = node;
        node->_next = nxt;
        node->_prev = &_dummy;
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        _dummy._next = node;
        ++_size;
        return node->value();
    }

    ~list() noexcept { clear(); }

    void clear() noexcept {
        ListNode *cur = _dummy._next;
        while ( cur != &_dummy ) {
            std::destroy_at(&cur->value());
            auto nxt = cur->_next;
            deleteNode(cur);
            cur = nxt;
        }
        _dummy._prev = _dummy._next = &_dummy;
        _size = 0;
    }

    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

      private:
        ListNode *_cur;

        friend list;

        explicit iterator(ListNode *cur) noexcept : _cur(cur) {}

      public:
        iterator() = default;

        iterator &operator++() noexcept {
            _cur = _cur->_next;
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        iterator &operator--() noexcept {
            _cur = _cur->_prev;
            return *this;
        }

        iterator operator--(int) noexcept {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        T &operator*() const noexcept { return _cur->value(); }

        bool operator!=(const iterator &other) const noexcept {
            return _cur != other._cur;
        }

        bool operator==(const iterator &other) const noexcept {
            return !(*this != other);
        }
    };

    struct const_iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T const *;
        using reference = T const &;

      private:
        const ListNode *_cur;

        friend list;

        explicit const_iterator(const ListNode *cur) noexcept : _cur(cur) {}

      public:
        const_iterator() = default;

        const_iterator(iterator other) noexcept : _cur(other._cur) {}

        explicit operator iterator() noexcept {
            return iterator{const_cast<ListNode *>(_cur)};
        }

        const_iterator &operator++() noexcept {
            _cur = _cur->_next;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator &operator--() noexcept {
            _cur = _cur->_prev;
            return *this;
        }

        const_iterator operator--(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const T &operator*() const noexcept { return _cur->value(); }

        bool operator!=(const const_iterator &other) const noexcept {
            return _cur != other._cur;
        }

        bool operator==(const const_iterator &other) const noexcept {
            return !(*this != other);
        }
    };

    iterator begin() noexcept { return iterator{_dummy._next}; }

    iterator end() noexcept { return iterator{&_dummy}; }

    const_iterator cbegin() const noexcept {
        return const_iterator{_dummy._next};
    }

    const_iterator cend() const noexcept { return const_iterator{&_dummy}; }

    const_iterator begin() const noexcept { return cbegin(); }

    const_iterator end() const noexcept { return cend(); }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin() noexcept {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return std::make_reverse_iterator(begin());
    }

    reverse_const_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    reverse_const_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    reverse_const_iterator rbegin() const noexcept { return crbegin(); }

    reverse_const_iterator rend() const noexcept { return crend(); }

    iterator erase(const_iterator pos) noexcept {
        ListNode *node = const_cast<ListNode *>(pos._cur);
        auto nxt = node->_next;
        auto pre = node->_prev;
        pre->_next = nxt;
        nxt->_prev = pre;
        std::destroy_at(&node->value());
        deleteNode(node);
        --_size;
        return iterator(nxt);
    }

    iterator erase(const_iterator first, const_iterator last) noexcept {
        while ( first != last ) { first = erase(first); }
        return iterator(first);
    }

    void pop_front() noexcept { erase(begin()); }

    void pop_back() noexcept { erase(std::prev(end())); }

    std::size_t remove(const T &val) noexcept {
        auto first = begin();
        auto last = end();
        std::size_t count = 0;
        while ( first != last ) {
            if ( *first == val ) {
                first = erase(first);
                ++count;
            } else {
                ++first;
            }
        }
        return count;
    }

    template <typename Pred>
    std::size_t remove_if(Pred &&pred) noexcept {
        auto first = begin();
        auto last = end();
        std::size_t count = 0;
        while ( first != last ) {
            if ( pred(*first) ) {
                first = erase(first);
                ++count;
            } else {
                ++first;
            }
        }
        return count;
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args &&...args) {
        ListNode *cur = newNode();
        ListNode *nxt = const_cast<ListNode *>(pos._cur);
        ListNode *pre = nxt->_prev;
        cur->_prev = pre;
        pre->_next = cur;
        cur->_next = nxt;
        nxt->_prev = cur;
        std::construct_at(&cur->value(), std::forward<Args>(args)...);
        ++_size;
        return iterator(cur);
    }

    iterator insert(const_iterator pos, const T &val) {
        return emplace(pos, val);
    }

    iterator insert(const_iterator pos, T &&val) {
        return emplace(pos, std::move(val));
    }

    iterator insert(const_iterator pos, std::size_t n, const T &val) {
        auto orig = pos;
        bool has_orig = false;
        while ( n ) {
            pos = emplace(pos, val);
            if ( !has_orig ) {
                has_orig = true;
                orig = pos;
            }
            ++pos;
            --n;
        }
        return iterator(
            orig); // Returns an iterator to the first element inserted.
    }

    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto orig = pos;
        bool has_orig = false;
        while ( first != last ) {
            pos = emplace(pos, *first);
            if ( !has_orig ) {
                has_orig = true;
                orig = pos;
            }
            ++pos;
            ++first;
        }
        return iterator(orig);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    void splice(const_iterator pos, list &&other) {
        insert(
            pos, std::make_move_iterator(other.begin()),
            std::make_move_iterator(other.end()));
    }

    Alloc get_allocator() const noexcept { return _alloc; }

    _LIBPENGCXX_DEFINE_COMPARISON(list);
};

} // namespace Marcus
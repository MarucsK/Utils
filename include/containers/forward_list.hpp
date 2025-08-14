#pragma once

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>

namespace Marcus {

template <typename T>
struct ForwardListBaseNode {
    ForwardListBaseNode *_next;

    inline T &value();
    inline const T &value() const;
};

template <typename T>
struct ForwardListValueNode : ForwardListBaseNode<T> {
    union {
        T _value;
    };
};

template <typename T>
inline T &ForwardListBaseNode<T>::value() {
    return static_cast<ForwardListValueNode<T> *>(this)->_value;
}

template <typename T>
inline const T &ForwardListBaseNode<T>::value() const {
    return static_cast<const ForwardListValueNode<T> *>(this)->_value;
}

template <typename T, typename Alloc = std::allocator<T>>
struct forward_list {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

private:
    using Node = ForwardListBaseNode<T>;
    using AllocNode = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ForwardListValueNode<T>>;

    Node _dummy;
    [[no_unique_address]] Alloc _alloc;

    Node *newNode() {
        AllocNode allocNode(_alloc);
        return std::allocator_traits<AllocNode>::allocate(allocNode, 1);
    }

    void deleteNode(Node *node) noexcept {
        AllocNode allocNode(_alloc);
        std::allocator_traits<AllocNode>::deallocate(
            allocNode, static_cast<ForwardListValueNode<T> *>(node), 1);
    }

    void destroyAndDeallocateNode(Node *node) noexcept {
        std::destroy_at(&node->value());
        deleteNode(node);
    }

public:
    forward_list() noexcept {
        _dummy._next = nullptr;
    }

    explicit forward_list(const Alloc &alloc) noexcept : _alloc(alloc) {
        _dummy._next = nullptr;
    }

    forward_list(forward_list &&other) noexcept
        : _alloc(std::move(other._alloc)) {
        _uninit_move_assign(std::move(other));
    }

    forward_list(forward_list &&other, const Alloc &alloc) noexcept
        : _alloc(alloc) {
        _uninit_move_assign(std::move(other));
    }

    forward_list &operator=(forward_list &&other) noexcept {
        if (this != &other) {
            clear();
            if constexpr (std::allocator_traits<Alloc>::
                              propagate_on_container_move_assignment::value) {
                _alloc = std::move(other._alloc);
            }
            _uninit_move_assign(std::move(other));
        }
        return *this;
    }

private:
    void _uninit_move_assign(forward_list &&other) noexcept {
        _dummy._next = other._dummy._next;
        other._dummy._next = nullptr;
    }

public:
    forward_list(const forward_list &other) : _alloc(other._alloc) {
        _uninit_assign(other.cbegin(), other.cend());
    }

    forward_list(const forward_list &other, const Alloc &alloc)
        : _alloc(alloc) {
        _uninit_assign(other.cbegin(), other.cend());
    }

    forward_list &operator=(const forward_list &other) {
        if (this != &other) {
            clear();
            if constexpr (std::allocator_traits<Alloc>::
                              propagate_on_container_copy_assignment::value) {
                _alloc = other._alloc;
            }
            _uninit_assign(other.cbegin(), other.cend());
        }
        return *this;
    }

    explicit forward_list(size_t n, const Alloc &alloc = Alloc())
        : _alloc(alloc) {
        _uninit_assign(n);
    }

    explicit forward_list(size_t n, const T &val, const Alloc &alloc = Alloc())
        : _alloc(alloc) {
        _uninit_assign(n, val);
    }

    template <std::input_iterator InputIt>
    forward_list(InputIt first, InputIt last, const Alloc &alloc = Alloc())
        : _alloc(alloc) {
        _uninit_assign(first, last);
    }

    forward_list(std::initializer_list<T> _ilist, const Alloc &alloc = Alloc())
        : forward_list(_ilist.begin(), _ilist.end(), alloc) {}

    forward_list &operator=(std::initializer_list<T> _ilist) {
        assign(_ilist);
        return *this;
    }

private:
    template <std::input_iterator InputIt>
    void _uninit_assign(InputIt first, InputIt last) {
        _dummy._next = nullptr;
        Node *cur_tail = &_dummy;
        while (first != last) {
            Node *node = newNode();
            std::construct_at(&node->value(), *first);
            node->_next = nullptr;
            cur_tail->_next = node;
            cur_tail = node;
            ++first;
        }
    }

    void _uninit_assign(size_t n) {
        _dummy._next = nullptr;
        Node *cur_tail = &_dummy;
        for (size_t i = 0; i < n; ++i) {
            Node *node = newNode();
            std::construct_at(&node->value());
            node->_next = nullptr;
            cur_tail->_next = node;
            cur_tail = node;
        }
    }

    void _uninit_assign(size_t n, const T &val) {
        _dummy._next = nullptr;
        Node *cur_tail = &_dummy;
        for (size_t i = 0; i < n; ++i) {
            Node *node = newNode();
            std::construct_at(&node->value(), val);
            node->_next = nullptr;
            cur_tail->_next = node;
            cur_tail = node;
        }
    }

public:
    T &front() noexcept {
        return _dummy._next->value();
    }

    const T &front() const noexcept {
        return _dummy._next->value();
    }

    bool empty() const noexcept {
        return _dummy._next == nullptr;
    }

    // std::size_t size() const noexcept {
    //     std::size_t count = 0;
    //     Node *cur = _dummy._next;
    //     while (cur != nullptr) {
    //         ++count;
    //         cur = cur->_next;
    //     }
    //     return count;
    // }

    constexpr std::size_t max_size() const noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    void clear() noexcept {
        Node *cur = _dummy._next;
        while (cur != nullptr) {
            auto nxt = cur->_next;
            destroyAndDeallocateNode(cur);
            cur = nxt;
        }
        _dummy._next = nullptr;
    }

    ~forward_list() noexcept {
        clear();
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

    void push_front(const T &val) {
        emplace_front(val);
    }

    void push_front(T &&val) {
        emplace_front(std::move(val));
    }

    template <typename... Args>
    T &emplace_front(Args &&...args)
        requires std::constructible_from<T, Args...>
    {
        Node *node = newNode();
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        node->_next = _dummy._next;
        _dummy._next = node;
        return node->value();
    }

    void pop_front() noexcept {
        Node *node_to_delete = _dummy._next;
        // if (node_to_delete == nullptr) {
        //     return;
        // }
        _dummy._next = node_to_delete->_next;
        destroyAndDeallocateNode(node_to_delete);
    }

    struct iterator {
        using iterator_category = std::forward_iterator_tag; // 前向迭代器
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

    private:
        Node *_cur;

        friend forward_list;

        explicit iterator(Node *cur) noexcept : _cur(cur) {}

    public:
        iterator &operator++() noexcept {
            _cur = _cur->_next;
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        T &operator*() const noexcept {
            return _cur->value();
        }

        pointer operator->() const noexcept {
            return &(_cur->value());
        }

        bool operator!=(const iterator &other) const noexcept {
            return _cur != other._cur;
        }

        bool operator==(const iterator &other) const noexcept {
            return _cur == other._cur;
        }
    };

    struct const_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

    private:
        const Node *_cur;

        friend forward_list;

        explicit const_iterator(const Node *cur) noexcept : _cur(cur) {}

    public:
        const_iterator() = default;

        const_iterator(iterator other) noexcept : _cur(other._cur) {}

        const_iterator &operator++() noexcept {
            _cur = _cur->_next;
            return *this;
        }

        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const T &operator*() const noexcept {
            return _cur->value();
        }

        const_pointer operator->() const noexcept {
            return &(_cur->value());
        }

        bool operator!=(const const_iterator &other) const noexcept {
            return _cur != other._cur;
        }

        bool operator==(const const_iterator &other) const noexcept {
            return _cur == other._cur;
        }
    };

    iterator before_begin() noexcept {
        return iterator{&_dummy};
    }

    const_iterator before_begin() const noexcept {
        return const_iterator{&_dummy};
    }

    const_iterator cbefore_begin() const noexcept {
        return const_iterator{&_dummy};
    }

    iterator begin() noexcept {
        return iterator{_dummy._next};
    }

    iterator end() noexcept {
        return iterator{nullptr};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{_dummy._next};
    }

    const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    const_iterator end() const noexcept {
        return cend();
    }

    template <typename... Args>
    iterator emplace_after(const_iterator pos, Args &&...args)
        requires std::constructible_from<T, Args...>
    {
        Node *prev = const_cast<Node *>(pos._cur);
        Node *new_node = newNode();
        std::construct_at(&new_node->value(), std::forward<Args>(args)...);

        new_node->_next = prev->_next;
        prev->_next = new_node;
        return iterator(new_node);
    }

    iterator insert_after(const_iterator pos, const T &val) {
        return emplace_after(pos, val);
    }

    iterator insert_after(const_iterator pos, T &&val) {
        return emplace_after(pos, std::move(val));
    }

    iterator insert_after(const_iterator pos, size_type n, const T &val) {
        iterator cur_pos = iterator(const_cast<Node *>(pos._cur));
        // if (cur_pos._cur == nullptr) {
        //     return cur_pos;
        // }
        iterator last_inserted_it = cur_pos;
        for (size_type i = 0; i < n; i++) {
            cur_pos = emplace_after(cur_pos, val);
            if (i == 0) {
                last_inserted_it = cur_pos;
            }
        }
        return last_inserted_it;
    }

    template <std::input_iterator InputIt>
    iterator insert_after(const_iterator pos, InputIt first, InputIt last) {
        iterator cur_pos = iterator(const_cast<Node *>(pos._cur));
        // if (cur_pos._cur == nullptr) {
        //     return cur_pos;
        // }
        iterator last_inserted_it = cur_pos;
        bool first_insertion = true;
        while (first != last) {
            cur_pos = emplace_after(cur_pos, *first);
            if (first_insertion) {
                last_inserted_it = cur_pos;
                first_insertion = false;
            }
            ++first;
        }
        return last_inserted_it;
    }

    iterator insert_after(const_iterator pos, std::initializer_list<T> ilist) {
        return insert_after(pos, ilist.begin(), ilist.end());
    }

    iterator erase_after(const_iterator pos) noexcept {
        Node *prev = const_cast<Node *>(pos._cur);
        Node *node_to_delete = prev->_next;
        if (node_to_delete == nullptr) {
            return end();
        }
        prev->_next = node_to_delete->_next;
        destroyAndDeallocateNode(node_to_delete);
        return iterator(prev->_next);
    }

    iterator erase_after(const_iterator first, const_iterator last) noexcept {
        Node *prev = const_cast<Node *>(first._cur);
        Node *current = prev->_next;
        while (current != last._cur) {
            Node *next_node = current->_next;
            destroyAndDeallocateNode(current);
            current = next_node;
        }
        prev->_next = const_cast<Node *>(last._cur);
        return iterator(const_cast<Node *>(last._cur));
    }

    size_type remove(const T &val) {
        size_type count = 0;
        Node *prev = &_dummy;
        Node *cur = _dummy._next;
        while (cur != nullptr) {
            if (cur->value() == val) {
                prev->_next = cur->_next;
                destroyAndDeallocateNode(cur);
                cur = prev->_next;
                ++count;
            } else {
                prev = cur;
                cur = cur->_next;
            }
        }
        return count;
    }

    template <typename Pred>
    size_type remove_if(Pred &&pred)
        requires std::predicate<Pred, T>
    {
        size_type count = 0;
        Node *prev = &_dummy;
        Node *cur = _dummy._next;
        while (cur != nullptr) {
            if (pred(cur->value())) {
                prev->_next = cur->_next;
                destroyAndDeallocateNode(cur);
                cur = prev->_next;
                ++count;
            } else {
                prev = cur;
                cur = cur->_next;
            }
        }
        return count;
    }

    void splice_after(const_iterator pos, forward_list &other) {
        if (this == &other || other.empty()) {
            return;
        }
        Node *prev = const_cast<Node *>(pos._cur);
        Node *other_first = other._dummy._next;
        Node *other_last = other_first;
        while (other_last->_next != nullptr) {
            other_last = other_last->_next;
        }

        other_last->_next = prev->_next;
        prev->_next = other_first;

        other._dummy._next = nullptr;
    }

    void splice_after(const_iterator pos, forward_list &&other) noexcept {
        if (this == &other || other.empty()) {
            return;
        }
        Node *prev = const_cast<Node *>(pos._cur);
        Node *other_first = other._dummy._next;
        Node *other_last = other_first;
        while (other_last->_next != nullptr) {
            other_last = other_last->_next;
        }

        other_last->_next = prev->_next;
        prev->_next = other_first;

        other._dummy._next = nullptr;
    }

    // other: it
    void splice_after(const_iterator pos, forward_list &other,
                      const_iterator it) noexcept {
        Node *prev_node = const_cast<Node *>(pos._cur);
        Node *other_prev = const_cast<Node *>(it._cur);

        Node *node_to_move = other_prev->_next;

        if (node_to_move == nullptr) {
            return;
        }

        other_prev->_next = node_to_move->_next;

        node_to_move->_next = prev_node->_next;
        prev_node->_next = node_to_move;
    }

    void splice_after(const_iterator pos, forward_list &&other,
                      const_iterator it) noexcept {
        Node *prev_node = const_cast<Node *>(pos._cur);
        Node *other_prev = const_cast<Node *>(it._cur);

        Node *node_to_move = other_prev->_next;

        if (node_to_move == nullptr) {
            return;
        }

        other_prev->_next = node_to_move->_next;

        node_to_move->_next = prev_node->_next;
        prev_node->_next = node_to_move;
    }

    // other: (before_first_to_move, before_last_to_move)
    void splice_after(const_iterator pos, forward_list &other,
                      const_iterator before_first_to_move,
                      const_iterator after_last_to_move) noexcept {
        Node *prev_node = const_cast<Node *>(pos._cur);
        Node *other_prev = const_cast<Node *>(before_first_to_move._cur);
        Node *first_to_move = other_prev->_next;
        Node *node_after_last_to_move =
            const_cast<Node *>(after_last_to_move._cur);

        if (first_to_move == node_after_last_to_move) {
            return;
        }

        Node *actual_last_moved = first_to_move;
        while (actual_last_moved->_next != node_after_last_to_move) {
            actual_last_moved = actual_last_moved->_next;
        }

        other_prev->_next = node_after_last_to_move;

        Node *temp_next = prev_node->_next;
        prev_node->_next = first_to_move;
        actual_last_moved->_next = temp_next;
    }

    void splice_after(const_iterator pos, forward_list &&other,
                      const_iterator before_first_to_move,
                      const_iterator after_last_to_move) noexcept {
        Node *prev_node = const_cast<Node *>(pos._cur);
        Node *other_prev = const_cast<Node *>(before_first_to_move._cur);
        Node *first_to_move = other_prev->_next;
        Node *node_after_last_to_move =
            const_cast<Node *>(after_last_to_move._cur);

        if (first_to_move == node_after_last_to_move) {
            return;
        }

        Node *actual_last_moved = first_to_move;
        while (actual_last_moved->_next != node_after_last_to_move) {
            actual_last_moved = actual_last_moved->_next;
        }

        other_prev->_next = node_after_last_to_move;

        Node *temp_next = prev_node->_next;
        prev_node->_next = first_to_move;
        actual_last_moved->_next = temp_next;
    }

    template <typename Compare = std::less<T>>
    void merge(forward_list &&other, Compare comp = Compare())
        requires std::strict_weak_order<Compare, T, T>
    {
        if (this == &other) {
            return;
        }
        Node *this_curr_prev = &_dummy;

        Node *this_curr = _dummy._next;
        Node *other_curr = other._dummy._next;

        while (this_curr != nullptr && other_curr != nullptr) {
            if (comp(other_curr->value(), this_curr->value())) {
                Node *node_to_move = other_curr;
                other_curr = other_curr->_next;

                node_to_move->_next = this_curr;
                this_curr_prev->_next = node_to_move;
                this_curr_prev = node_to_move;
            } else {
                this_curr_prev = this_curr;
                this_curr = this_curr->_next;
            }
        }
        if (other_curr != nullptr) {
            this_curr_prev->_next = other_curr;
        }
        other._dummy._next = nullptr;
    }

    template <typename Compare = std::less<T>>
    void sort(Compare comp = Compare())
        requires std::strict_weak_order<Compare, T, T>
    {
        if (empty() || _dummy._next->_next == nullptr) {
            return;
        }
        Node *head = _dummy._next;
        Node *slow = head;
        Node *fast = head->_next;

        while (fast != nullptr && fast->_next != nullptr) {
            slow = slow->_next;
            fast = fast->_next->_next;
        }

        forward_list left_half;
        left_half._dummy._next = head;
        forward_list right_half;
        right_half._dummy._next = slow->_next;

        slow->_next = nullptr;

        left_half.sort(comp);
        right_half.sort(comp);

        _dummy._next = nullptr;
        Node *cur_tail = &_dummy;

        Node *l_curr = left_half._dummy._next;
        Node *r_curr = right_half._dummy._next;

        while (l_curr != nullptr && r_curr != nullptr) {
            if (comp(r_curr->value(), l_curr->value())) {
                cur_tail->_next = r_curr;
                r_curr = r_curr->_next;
            } else {
                cur_tail->_next = l_curr;
                l_curr = l_curr->_next;
            }
            cur_tail = cur_tail->_next;
        }

        if (l_curr != nullptr) {
            cur_tail->_next = l_curr;
        } else if (r_curr != nullptr) {
            cur_tail->_next = r_curr;
        }

        left_half._dummy._next = nullptr;
        right_half._dummy._next = nullptr;
    }

    template <typename BinaryPredicate = std::equal_to<T>>
    size_type unique(BinaryPredicate pred = BinaryPredicate()) {
        if (empty()) {
            return 0;
        }
        size_type count = 0;
        Node *prev = _dummy._next;
        Node *curr = prev->_next;

        while (curr != nullptr) {
            if (pred(prev->value(), curr->value())) {
                prev->_next = curr->_next;
                destroyAndDeallocateNode(curr);
                curr = prev->_next;
                ++count;
            } else {
                prev = curr;
                curr = curr->_next;
            }
        }
        return count;
    }

    void reverse() noexcept {
        if (empty() || _dummy._next->_next == nullptr) {
            return;
        }
        Node *prev = nullptr;
        Node *curr = _dummy._next;
        Node *next_node = nullptr;

        while (curr != nullptr) {
            next_node = curr->_next;
            curr->_next = prev;
            prev = curr;
            curr = next_node;
        }
        _dummy._next = prev;
    }

    void swap(forward_list &other) noexcept {
        using std::swap;
        swap(_dummy._next, other._dummy._next);
        if constexpr (std::allocator_traits<
                          Alloc>::propagate_on_container_swap::value) {
            swap(_alloc, other._alloc);
        }
    }

    Alloc get_allocator() const noexcept {
        return _alloc;
    }

    bool operator==(const forward_list &other) const noexcept {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    bool operator!=(const forward_list &other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const forward_list &other) const noexcept {
        return std::lexicographical_compare(begin(), end(), other.begin(),
                                            other.end());
    }

    bool operator>(const forward_list &other) const noexcept {
        return other < *this;
    }

    bool operator<=(const forward_list &other) const noexcept {
        return !(*this > other);
    }

    bool operator>=(const forward_list &other) const noexcept {
        return !(*this < other);
    }
};

template <typename T, typename Alloc>
void swap(forward_list<T, Alloc> &lhs, forward_list<T, Alloc> &rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace Marcus

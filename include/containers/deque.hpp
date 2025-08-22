#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#if __cpp_lib_three_way_comparison
# include <compare>
#endif

namespace Marcus {

template <typename _Tp, typename _Alloc>
class deque;

template <typename _Tp>
static constexpr std::size_t _deque_get_block_size() noexcept {
    if (sizeof(_Tp) < 32) {
        return 512 / sizeof(_Tp);
    }
    return 16;
}

template <typename _Tp, typename _Ref, typename _Ptr>
struct deque_iterator {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = _Tp;
    using difference_type = std::ptrdiff_t;
    using pointer = _Ptr;
    using reference = _Ref;
    using map_pointer = _Tp **;

    pointer _current;
    pointer _first;
    pointer _last;
    map_pointer _node;

    static constexpr std::size_t _block_size = _deque_get_block_size<_Tp>();

    deque_iterator() noexcept
        : _current(nullptr),
          _first(nullptr),
          _last(nullptr),
          _node(nullptr) {}

    deque_iterator(pointer __x, map_pointer __y) noexcept
        : _current(__x),
          _first(*__y),
          _last(*__y + _block_size),
          _node(__y) {}

    template <typename _OtherRef, typename _OtherPtr>
    deque_iterator(
        const deque_iterator<_Tp, _OtherRef, _OtherPtr> &__other) noexcept
        : _current(__other._current),
          _first(__other._first),
          _last(__other._last),
          _node(__other._node) {}

    reference operator*() const noexcept {
        return *_current;
    }

    pointer operator->() const noexcept {
        return _current;
    }

    deque_iterator &operator++() noexcept {
        ++_current;
        if (_current == _last) {
            _set_node(_node + 1);
            _current = _first;
        }
        return *this;
    }

    deque_iterator operator++(int) noexcept {
        deque_iterator __temp = *this;
        ++(*this);
        return __temp;
    }

    deque_iterator &operator--() noexcept {
        if (_current == _first) {
            _set_node(_node - 1);
            _current = _last;
        }
        --_current;
        return *this;
    }

    deque_iterator operator--(int) noexcept {
        deque_iterator __temp = *this;
        --(*this);
        return __temp;
    }

    deque_iterator &operator+=(difference_type __n) noexcept {
        const difference_type __offset =
            __n + (_current - _first); // 閻╂瓕绶濇禍宸乫irst閻ㄥ嫬浜哥粔濠氬櫤
        if (__offset >= 0 &&
            __offset < static_cast<difference_type>(_block_size)) {
            _current += __n;
        } else {
            const difference_type __node_offset =
                __offset > 0 ? __offset / _block_size
                             : -((-__offset - 1) / _block_size) - 1;
            _set_node(_node + __node_offset);
            _current = _first + (__offset - __node_offset * _block_size);
        }
        return *this;
    }

    deque_iterator operator+(difference_type __n) const noexcept {
        deque_iterator __temp = *this;
        return __temp += __n;
    }

    deque_iterator operator-=(difference_type __n) noexcept {
        return *this += -__n;
    }

    deque_iterator operator-(difference_type __n) const noexcept {
        deque_iterator __temp = *this;
        return __temp -= __n;
    }

    reference operator[](difference_type __n) const noexcept {
        return *(*this + __n);
    }

private:
    void _set_node(map_pointer __new_node) noexcept {
        _node = __new_node;
        _first = *__new_node;
        _last = _first + _block_size;
    }

    template <typename _T, typename _R, typename _P>
    friend struct deque_iterator;

    template <typename _T, typename _A>
    friend class deque;
};

template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator==(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                       const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return __x._current == __y._current;
}

#if __cpp_lib_three_way_comparison
template <typename _Tp, typename _Ref, typename _Ptr>
inline std::strong_ordering
operator<=>(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
            const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    if (__x._node != __y._node) {
        return __x._node <=> __y._node;
    }
    return __x._current <=> __y._current;
}
#else
template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator!=(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                       const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return !(__x == __y);
}

template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator<(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                      const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return (__x._node == __y._node) ? (__x._current < __y._current)
                                    : (__x._node < __y._node);
}

template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator>(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                      const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return __y < __x;
}

template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator<=(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                       const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return !(__y < __x);
}

template <typename _Tp, typename _Ref, typename _Ptr>
inline bool operator>=(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
                       const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    return !(__x < __y);
}
#endif

template <typename _Tp, typename _Ref, typename _Ptr>
inline typename deque_iterator<_Tp, _Ref, _Ptr>::difference_type
operator-(const deque_iterator<_Tp, _Ref, _Ptr> &__x,
          const deque_iterator<_Tp, _Ref, _Ptr> &__y) noexcept {
    using iter = deque_iterator<_Tp, _Ref, _Ptr>;
    if (__x._node == __y._node) {
        return __x._current - __y._current;
    }
    return static_cast<typename iter::difference_type>(iter::_block_size) *
               (__x._node - __y._node - 1) +
           (__x._current - __x._first) + (__y._last - __y._current);
}

// 閹绘劒绶电€靛湱袨閸旂姵纭?
template <typename _Tp, typename _Ref, typename _Ptr>
inline deque_iterator<_Tp, _Ref, _Ptr>
operator+(typename deque_iterator<_Tp, _Ref, _Ptr>::difference_type __n,
          const deque_iterator<_Tp, _Ref, _Ptr> &__x) noexcept {
    return __x + __n;
}

template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
class deque {
public:
    using value_type = _Tp;
    using allocator_type = _Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = _Tp &;
    using const_reference = const _Tp &;
    using pointer = typename std::allocator_traits<_Alloc>::pointer;
    using const_pointer = typename std::allocator_traits<_Alloc>::const_pointer;

    using iterator = deque_iterator<_Tp, _Tp &, _Tp *>;
    using const_iterator = deque_iterator<_Tp, const _Tp &, const _Tp *>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using _Map_alloc_type =
        typename std::allocator_traits<_Alloc>::template rebind_alloc<pointer>;
    using _Map_alloc_traits = std::allocator_traits<_Map_alloc_type>;

    pointer *_map;
    size_type _map_size;
    iterator _start;
    iterator _finish;
    [[no_unique_address]] allocator_type _alloc;

    static constexpr size_type _block_size = _deque_get_block_size<_Tp>();

private:
    pointer _allocate_block() {
        return std::allocator_traits<allocator_type>::allocate(_alloc,
                                                               _block_size);
    }

    void _deallocate_block(pointer __p) {
        std::allocator_traits<allocator_type>::deallocate(_alloc, __p,
                                                          _block_size);
    }

    pointer *_allocate_map(size_type __n) {
        _Map_alloc_type __map_alloc(get_allocator());
        return _Map_alloc_traits::allocate(__map_alloc, __n);
    }

    void _deallocate_map(pointer *__p, size_type __n) {
        _Map_alloc_type __map_alloc(get_allocator());
        _Map_alloc_traits::deallocate(__map_alloc, __p, __n);
    }

    void _create_map_and_nodes(size_type __num_elements) {
        const size_type __num_nodes = __num_elements / _block_size + 1;
        _map_size = std::max(static_cast<size_type>(8), __num_nodes + 2);
        _map = _allocate_map(_map_size);

        pointer *__nstart = _map + (_map_size - __num_nodes) / 2;
        pointer *__nfinish = __nstart + __num_nodes - 1;

        try {
            for (pointer *__cur = __nstart; __cur <= __nfinish; ++__cur) {
                *__cur = _allocate_block();
            }
        } catch (...) {
            for (pointer *__cur = __nstart; *__cur != nullptr; ++__cur) {
                _deallocate_block(*__cur);
            }
            _deallocate_map(_map, _map_size);
            throw;
        }

        _start._set_node(__nstart);
        _start._current = _start._first;
        _finish._set_node(__nfinish);
        _finish._current = _finish._first + __num_elements % _block_size;
    }

    // [__first, __last)
    void _destroy_elements(iterator __first, iterator __last) {
        // for (iterator __it = __first; __it != __last; ++__it) {
        //     std::destroy_at(__it._current);
        // }
        for (auto *__node = __first._node + 1; __node < __last._node;
             ++__node) {
            for (pointer __p = *__node; __p < *__node + _block_size; ++__p) {
                std::destroy_at(__p);
            }
        }
        if (__first._node != __last._node) {
            for (pointer __p = __first._current; __p < __first._last; ++__p) {
                std::destroy_at(__p);
            }
            for (pointer __p = __last._first; __p < __last._current; ++__p) {
                std::destroy_at(__p);
            }
        } else {
            for (pointer __p = __first._current; __p < __last._current; ++__p) {
                std::destroy_at(__p);
            }
        }
    }

    void _deallocate_all() noexcept {
        if (_map) {
            for (pointer *__node = _start._node; __node <= _finish._node;
                 ++__node) {
                _deallocate_block(*__node);
            }
            _deallocate_map(_map, _map_size);
        }
    }

    void _reallocate_map(size_type __nodes_to_add, bool __add_at_front) {
        size_type __old_num_nodes = _finish._node - _start._node + 1;
        size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

        pointer *__new_map;
        if (_map_size > 2 * __new_num_nodes) {
            __new_map = _map;
            size_type __nodes_before =
                _start._node -
                _map; // 閸︹暃start._node娑斿澧犻張澶婎樋鐏忔垳閲滅粚娲＝閻ㄥ嫬娼?
            if (__nodes_before > __new_num_nodes / 2) {
                pointer *__new_start_node =
                    _map + (_map_size - __new_num_nodes) / 2;
                std::move(_start._node, _finish._node + 1, __new_start_node);
                _start._set_node(__new_start_node);
                _finish._set_node(__new_start_node + __old_num_nodes - 1);
            }
        } else {
            size_type __new_map_size =
                _map_size + std::max(_map_size, __nodes_to_add) + 2;
            __new_map = _allocate_map(__new_map_size);
            pointer *__new_start_node =
                __new_map + (__new_map_size - __new_num_nodes) / 2;
            if (__add_at_front) {
                __new_start_node += __nodes_to_add;
            }
            std::move(_start._node, _finish._node + 1, __new_start_node);
            _deallocate_map(_map, _map_size);

            _map = __new_map;
            _map_size = __new_map_size;
            _start._set_node(__new_start_node);
            _finish._set_node(__new_start_node + __old_num_nodes - 1);
        }
    }

    void _push_back_aux() {
        if (_finish._node + 1 == _map + _map_size) {
            _reallocate_map(1, false);
        }
        *(_finish._node + 1) = _allocate_block();
        _finish._set_node(_finish._node + 1);
        _finish._current = _finish._first;
    }

    void _push_front_aux() {
        if (_start._node == _map) {
            _reallocate_map(1, true);
        }
        *(_start._node - 1) = _allocate_block();
        _start._set_node(_start._node - 1);
        _start._current = _start._last - 1;
    }

    void _pop_back_aux() {
        _deallocate_block(_finish._first);
        _finish._set_node(_finish._node - 1);
        _finish._current = _finish._last - 1;
    }

    void _pop_front_aux() {
        _deallocate_block(_start._first);
        _start._set_node(_start._node + 1);
        _start._current = _start._first;
    }

    size_type _capacity_front() const noexcept {
        if (!_map) {
            return 0;
        }
        const size_type __slots_in_start_node = _start._current - _start._first;
        const size_type __nodes_before_start = _start._node - _map;
        return __nodes_before_start * _block_size + __slots_in_start_node;
    }

    size_type _capacity_back() const noexcept {
        if (!_map) {
            return 0;
        }
        const size_type __slots_in_finish_node =
            _finish._last - _finish._current;
        const size_type __nodes_after_finish =
            (_map + _map_size) - _finish._node - 1;
        return __nodes_after_finish * _block_size + __slots_in_finish_node;
    }

    size_type _capacity() const noexcept {
        return _capacity_front() + size() + _capacity_back();
    }

    iterator _get_iterator(const_iterator __cit) noexcept {
        return begin() + (__cit - cbegin());
    }

public:
    deque() noexcept(noexcept(allocator_type()))
        : _map(nullptr),
          _map_size(0),
          _start(),
          _finish(),
          _alloc() {}

    explicit deque(const allocator_type &__alloc) noexcept
        : _map(nullptr),
          _map_size(0),
          _start(),
          _finish(),
          _alloc(__alloc) {}

    explicit deque(size_type __n,
                   const allocator_type &__alloc = allocator_type())
        : _alloc(__alloc) {
        _create_map_and_nodes(__n);
        for (iterator __cur = _start; __cur != _finish; ++__cur) {
            std::construct_at(__cur._current);
        }
    }

    deque(size_type __n, const_reference __val,
          const allocator_type &__alloc = allocator_type())
        : _alloc(__alloc) {
        _create_map_and_nodes(__n);
        iterator __cur = _start;
        try {
            for (; __cur != _finish; ++__cur) {
                std::construct_at(__cur._current, __val);
            }
        } catch (...) {
            _destroy_elements(_start, __cur);
            _deallocate_all();
            throw;
        }
    }

    // template <
    //     typename _InputIt,
    //     typename = std::enable_if_t<std::is_convertible_v<
    //         typename std::iterator_traits<_InputIt>::iterator_category,
    //         std::input_iterator_tag>>>
    template <std::input_iterator _InputIt>
    deque(_InputIt __first, _InputIt __last,
          const allocator_type &__alloc = allocator_type())
        : _alloc(__alloc) {
        const size_type __n = std::distance(__first, __last);
        _create_map_and_nodes(__n);
        iterator __cur = _start;
        try {
            for (; __first != __last; ++__first, ++__cur) {
                std::construct_at(__cur._current, *__first);
            }
        } catch (...) {
            _destroy_elements(_start, __cur);
            _deallocate_all();
            throw;
        }
    }

    deque(const deque &__other)
        : _alloc(std::allocator_traits<allocator_type>::
                     select_on_container_copy_construction(
                         __other.get_allocator())) {
        const size_type __n = __other.size();
        _create_map_and_nodes(__n);
        iterator __cur = _start;
        const_iterator __other_cur = __other.begin();
        try {
            for (; __cur != _finish; ++__cur, ++__other_cur) {
                std::construct_at(__cur._current, *__other_cur);
            }
        } catch (...) {
            _destroy_elements(_start, __cur);
            _deallocate_all();
            throw;
        }
    }

    deque(deque &&__other) noexcept
        : _map(__other._map),
          _map_size(__other._map_size),
          _start(__other._start),
          _finish(__other._finish),
          _alloc(std::move(__other._alloc)) {
        __other._map = nullptr;
        __other._map_size = 0;
        __other._start = iterator();
        __other._finish = iterator();
    }

    deque(std::initializer_list<_Tp> __il,
          const allocator_type &__alloc = allocator_type())
        : deque(__il.begin(), __il.end(), __alloc) {}

    ~deque() noexcept {
        if (_map) {
            _destroy_elements(_start, _finish);
            _deallocate_all();
        }
    }

    deque &operator=(const deque &__other) {
        if (this == &__other) [[unlikely]] {
            return *this;
        }
        clear();
        if (std::allocator_traits<allocator_type>::
                propagate_on_container_copy_assignment::value) {
            _alloc = __other._alloc;
        }
        assign(__other.begin(), __other.end());
        return *this;
    }

    // deque &operator=(deque &&__other) noexcept {
    //     if (this == &__other) [[unlikely]] {
    //         return *this;
    //     }
    //     clear();
    //     _deallocate_all();
    //     _map = __other._map;
    //     _map_size = __other._map_size;
    //     _start = __other._start;
    //     _finish = __other._finish;
    //     _alloc = std::move(__other._alloc);
    //     __other._map = nullptr;
    //     __other._map_size = 0;
    //     return *this;
    // }

    deque &operator=(deque &&__other) noexcept(
        std::allocator_traits<allocator_type>::is_always_equal::value) {
        if (this == &__other) [[unlikely]] {
            return *this;
        }
        if (std::allocator_traits<allocator_type>::
                propagate_on_container_move_assignment::value) {
            clear();
            _deallocate_all();

            _map = __other._map;
            _map_size = __other._map_size;
            _start = __other._start;
            _finish = __other._finish;
            _alloc = std::move(__other._alloc);
            __other._map = nullptr;
            __other._map_size = 0;
            __other._start = iterator();
            __other._finish = iterator();
        } else {
            if (_alloc == __other._alloc) {
                clear();
                _deallocate_all();

                _map = __other._map;
                _map_size = __other._map_size;
                _start = __other._start;
                _finish = __other._finish;
                __other._map = nullptr;
                __other._map_size = 0;
                __other._start = iterator();
                __other._finish = iterator();
            } else {
                assign(std::make_move_iterator(__other.begin()),
                       std::make_move_iterator(__other.end()));
            }
        }
        return *this;
    }

    deque &operator=(std::initializer_list<_Tp> __ilist) {
        assign(__ilist.begin(), __ilist.end());
        return *this;
    }

    void assign(size_type __n, const_reference __val) {
        clear();
        if (__n > _capacity()) {
            _deallocate_all();
            _create_map_and_nodes(__n);
        }
        resize(__n, __val);
    }

    // template <
    //     typename _InputIt,
    //     typename = std::enable_if_t<std::is_convertible_v<
    //         typename std::iterator_traits<_InputIt>::iterator_category,
    //         std::input_iterator_tag>>>
    template <std::input_iterator _InputIt>
    void assign(_InputIt __first, _InputIt __last) {
        clear();
        for (; __first != __last; ++__first) {
            emplace_back(*__first);
        }
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    reference at(size_type __i) {
        if (__i >= size()) [[unlikely]] {
            throw std::out_of_range("deque::at");
        }
        return (*this)[__i];
    }

    const_reference at(size_type __i) const {
        if (__i >= size()) [[unlikely]] {
            throw std::out_of_range("deque::at");
        }
        return (*this)[__i];
    }

    reference operator[](size_type __i) noexcept {
        return _start[static_cast<difference_type>(__i)];
    }

    const_reference operator[](size_type __i) const noexcept {
        return _start[static_cast<difference_type>(__i)];
    }

    reference front() noexcept {
        return *_start;
    }

    const_reference front() const noexcept {
        return *_start;
    }

    reference back() noexcept {
        iterator __tmp = _finish;
        --__tmp;
        return *__tmp;
    }

    const_reference back() const noexcept {
        const_iterator __tmp = _finish;
        --__tmp;
        return *__tmp;
    }

    iterator begin() noexcept {
        return _start;
    }

    const_iterator begin() const noexcept {
        return _start;
    }

    const_iterator cbegin() const noexcept {
        return _start;
    }

    iterator end() noexcept {
        return _finish;
    }

    const_iterator end() const noexcept {
        return _finish;
    }

    const_iterator cend() const noexcept {
        return _finish;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    bool empty() const noexcept {
        return _finish == _start;
    }

    size_type size() const noexcept {
        if (_map == nullptr) {
            return 0;
        }
        return _finish - _start;
    }

    size_type max_size() const noexcept {
        return std::allocator_traits<allocator_type>::max_size(_alloc);
    }

    void shrink_to_fit() {
        if (empty()) {
            _deallocate_all();
            _map = nullptr;
            _map_size = 0;
            _start = _finish = iterator();
            return;
        }
        const size_type __blocks_in_use = (_finish._node - _start._node) + 1;
        const size_type __ideal_blocks =
            (size() + _block_size - 1) /
            _block_size; // 鏈€灏戝灏戝潡鑳藉绾冲綋鍓嶆墍鏈夊厓绱?
        if (__blocks_in_use == __ideal_blocks &&
            _map_size < __blocks_in_use * 2) {
            return;
        }
        deque<_Tp, _Alloc> __temp(std::make_move_iterator(begin()),
                                  std::make_move_iterator(end()),
                                  get_allocator());
        this->swap(__temp);
    }

    void resize(size_type __n) {
        const size_type __old_size = size();
        if (__n > __old_size) {
            for (size_type i = 0; i < __n - __old_size; ++i) {
                emplace_back();
            }
        } else if (__n < __old_size) {
            for (size_type i = 0; i < __old_size - __n; ++i) {
                pop_back();
            }
        }
    }

    void resize(size_type __n, const_reference __val) {
        const size_type __old_size = size();
        if (__n > __old_size) {
            for (size_type i = 0; i < __n - __old_size; ++i) {
                push_back(__val);
            }
        } else if (__n < __old_size) {
            for (size_type i = 0; i < __old_size - __n; ++i) {
                pop_back();
            }
        }
    }

    void clear() noexcept {
        if (_map) {
            _destroy_elements(_start, _finish);
            for (pointer *__node = _start._node + 1; __node <= _finish._node;
                 ++__node) {
                _deallocate_block(*__node);
            }
            _finish = _start;
        }
    }

    template <typename... _Args>
    reference emplace_front(_Args &&...__args) {
        if (_map == nullptr) {
            _create_map_and_nodes(0);
        }
        if (_start._current != _start._first) {
            --_start._current;
            std::construct_at(_start._current, std::forward<_Args>(__args)...);
        } else {
            _push_front_aux();
            std::construct_at(_start._current, std::forward<_Args>(__args)...);
        }
        return front();
    }

    template <typename... _Args>
    reference emplace_back(_Args &&...__args) {
        if (_map == nullptr) {
            _create_map_and_nodes(0);
        }
        if (_finish._current != _finish._last) {
            std::construct_at(_finish._current, std::forward<_Args>(__args)...);
            ++_finish._current;
        } else {
            _push_back_aux();
            std::construct_at(_finish._current, std::forward<_Args>(__args)...);
            ++_finish._current;
        }
        return back();
    }

    template <typename... _Args>
    iterator emplace(const_iterator __pos, _Args &&...__args) {
        if (__pos == cbegin()) {
            emplace_front(std::forward<_Args>(__args)...);
            return _start;
        }
        if (__pos == cend()) {
            emplace_back(std::forward<_Args>(__args)...);
            return _finish - 1;
        }
        if (empty()) {
            emplace_back(std::forward<_Args>(__args)...);
            return _start;
        }

        iterator __p = _get_iterator(__pos);
        const difference_type __index = __p - _start;

        if (static_cast<size_type>(__index) < size() / 2) {
            emplace_front(front());
            iterator __insert_pos = begin() + 1;
            std::move(__insert_pos, begin() + __index + 1, begin());
            // *(_start + __index) = value_type(std::forward<_Args>(__args)...);
            std::destroy_at(std::addressof(*(_start + __index)));
            std::construct_at(std::addressof(*(_start + __index)),
                              std::forward<_Args>(__args)...);
            return _start + __index;
        } else {
            emplace_back(back());
            iterator __actual_pos = _start + __index;
            std::move_backward(__actual_pos, end() - 2, end() - 1);
            // *__actual_pos = value_type(std::forward<_Args>(__args)...);
            std::destroy_at(std::addressof(*__actual_pos));
            std::construct_at(std::addressof(*__actual_pos),
                              std::forward<_Args>(__args)...);
            return __actual_pos;
        }
    }

    void push_front(const _Tp &__val) {
        emplace_front(__val);
    }

    void push_front(_Tp &&__val) {
        emplace_front(std::move(__val));
    }

    void push_back(const _Tp &__val) {
        emplace_back(__val);
    }

    void push_back(_Tp &&__val) {
        emplace_back(std::move(__val));
    }

    iterator insert(const_iterator __pos, const _Tp &__value) {
        return emplace(__pos, __value);
    }

    iterator insert(const_iterator __pos, _Tp &&__value) {
        return emplace(__pos, std::move(__value));
    }

    iterator insert(const_iterator __pos, size_type __count,
                    const _Tp &__value) {
        if (__count == 0) {
            return _get_iterator(__pos);
        }
        const difference_type __offset = __pos - cbegin();
        const size_type __old_size = size();
        if (__pos == cbegin()) {
            for (size_type i = 0; i < __count; ++i) {
                push_front(__value);
            }
            return begin();
        }
        if (__pos == cend()) {
            for (size_type i = 0; i < __count; ++i) {
                push_back(__value);
            }
            return end() - __count;
        }
        // general: create a tempory deque, and swap
        deque __tmp(get_allocator());
        __tmp._create_map_and_nodes(__old_size + __count);

        auto __dest = std::uninitialized_move(
            std::make_move_iterator(begin()),
            std::make_move_iterator(_get_iterator(__pos)), __tmp.begin());
        std::uninitialized_fill_n(__dest, __count, __value);
        std::uninitialized_move(std::make_move_iterator(_get_iterator(__pos)),
                                std::make_move_iterator(end()),
                                __tmp.begin() + __offset + __count);
        this->swap(__tmp);
        return begin() + __offset;
    }

    template <std::input_iterator _InputIt>
    iterator insert(const_iterator __pos, _InputIt __first, _InputIt __last) {
        if constexpr (!std::is_base_of_v<std::forward_iterator_tag,
                                         typename std::iterator_traits<
                                             _InputIt>::iterator_category>) {
            iterator __p = _get_iterator(__pos);
            const difference_type __offset = __p - begin();
            for (; __first != __last; ++__first) {
                __p = emplace(__p, *__first);
                ++__p;
            }
            return begin() + __offset;
        } else {
            const size_type __count = std::distance(__first, __last);

            if (__count == 0) {
                return _get_iterator(__pos);
            }

            const difference_type __offset = __pos - cbegin();
            const size_type __old_size = size();

            deque __tmp(get_allocator());
            __tmp._create_map_and_nodes(__old_size + __count);

            auto __dest = std::uninitialized_move(
                std::make_move_iterator(begin()),
                std::make_move_iterator(_get_iterator(__pos)), __tmp.begin());
            std::uninitialized_copy(__first, __last, __dest);
            std::uninitialized_move(
                std::make_move_iterator(_get_iterator(__pos)),
                std::make_move_iterator(end()),
                __tmp.begin() + __offset + __count);

            this->swap(__tmp);
            return begin() + __offset;
        }
    }

    iterator insert(const_iterator __pos, std::initializer_list<_Tp> __ilist) {
        return insert(__pos, __ilist.begin(), __ilist.end());
    }

    void pop_front() noexcept {
        if (_start._current != _start._last - 1) {
            std::destroy_at(_start._current);
            ++_start._current;
        } else {
            std::destroy_at(_start._current);
            _pop_front_aux();
        }
    }

    void pop_back() noexcept {
        if (_finish._current != _finish._first) {
            --_finish._current;
            std::destroy_at(_finish._current);
        } else {
            _pop_back_aux();
            std::destroy_at(_finish._current);
        }
    }

    iterator erase(const_iterator __pos) {
        iterator __p = _get_iterator(__pos);
        const difference_type __index = __p - _start;

        if (static_cast<size_type>(__index) < size() / 2) {
            std::move_backward(_start, __p, __p + 1);
            pop_front();
        } else {
            std::move(__p + 1, _finish, __p);
            pop_back();
        }
        return _start + __index;
    }

    iterator erase(const_iterator __first, const_iterator __last) {
        if (__first == __last) {
            return _get_iterator(__first);
        }
        if (__first == cbegin() && __last == cend()) {
            clear();
            return end();
        }

        iterator __f = _get_iterator(__first);
        iterator __l = _get_iterator(__last);

        const difference_type __n = __l - __f;
        const difference_type __elems_before = __f - _start;
        const difference_type __elems_after = _finish - __l;

        if (__elems_before < __elems_after) {
            std::move_backward(_start, __f, __l);
            iterator __new_start = _start + __n;
            _destroy_elements(_start, __new_start);
            for (pointer *__node = _start._node; __node < __new_start._node;
                 ++__node) {
                _deallocate_block(*__node);
            }
            _start = __new_start;
            return __l;
        } else {
            iterator __new_finish = std::move(__l, _finish, __f);
            _destroy_elements(__new_finish, _finish);
            for (pointer *__node = __new_finish._node + 1;
                 __node <= _finish._node; ++__node) {
                _deallocate_block(*__node);
            }
            _finish = __new_finish;
            return __f;
        }
    }

    void swap(deque &__other) noexcept {
        std::swap(_map, __other._map);
        std::swap(_map_size, __other._map_size);
        std::swap(_start, __other._start);
        std::swap(_finish, __other._finish);
        if (std::allocator_traits<
                allocator_type>::propagate_on_container_swap::value) {
            std::swap(_alloc, __other._alloc);
        }
    }
};

template <typename _Tp, typename _Alloc>
void swap(deque<_Tp, _Alloc> &__lhs, deque<_Tp, _Alloc> &__rhs) {
    __lhs.swap(__rhs);
}

#if __cpp_lib_three_way_comparison
template <class _Tp, class _Alloc>
inline std::strong_ordering operator<=>(const Marcus::deque<_Tp, _Alloc> &__x,
                                        const Marcus::deque<_Tp, _Alloc> &__y) {
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(),
                                                  __y.begin(), __y.end());
}
#else
template <class _Tp, class _Alloc>
inline bool operator!=(const Marcus::deque<_Tp, _Alloc> &__x,
                       const Marcus::deque<_Tp, _Alloc> &__y) {
    return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool operator<(const Marcus::deque<_Tp, _Alloc> &__x,
                      const Marcus::deque<_Tp, _Alloc> &__y) {
    return std::lexicographical_compare(__x.begin(), __x.end(), __y.begin(),
                                        __y.end());
}

template <class _Tp, class _Alloc>
inline bool operator>(const Marcus::deque<_Tp, _Alloc> &__x,
                      const Marcus::deque<_Tp, _Alloc> &__y) {
    return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool operator<=(const Marcus::deque<_Tp, _Alloc> &__x,
                       const Marcus::deque<_Tp, _Alloc> &__y) {
    return !(__y < __x);
}

template <class _Tp, class _Alloc>
inline bool operator>=(const Marcus::deque<_Tp, _Alloc> &__x,
                       const Marcus::deque<_Tp, _Alloc> &__y) {
    return !(__x < __y);
}
#endif

template <typename _Tp, typename _Alloc>
inline bool operator==(const deque<_Tp, _Alloc> &__x,
                       const deque<_Tp, _Alloc> &__y) {
    return __x.size() == __y.size() &&
           std::equal(__x.begin(), __x.end(), __y.begin());
}

} // namespace Marcus

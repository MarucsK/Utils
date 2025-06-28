#pragma once

#include <memory>
#include <limits>
#include <initializer_list>

#include <core/_common.hpp>

namespace Marcus {

template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
struct vector {
  public:
    using value_type = _Tp;
    using allocator_type = _Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = _Tp *;
    using const_pointer = _Tp const *;
    using reference = _Tp &;
    using const_reference = _Tp const &;
    using iterator = _Tp *;
    using const_iterator = _Tp const *;
    using reverse_iterator = std::reverse_iterator<_Tp *>;
    using const_reverse_iterator = std::reverse_iterator<_Tp const *>;

  private:
    _Tp *_data;
    std::size_t _size;
    std::size_t _cap;
    [[no_unique_address]] _Alloc _alloc;

  public:
    vector() noexcept {
        _data = nullptr;
        _size = 0;
        _cap = 0;
    }

    vector(std::initializer_list<_Tp> _list, const _Alloc &alloc = _Alloc())
        : vector(_list.begin(), _list.end(), alloc) {}

    explicit vector(std::size_t _n, const _Alloc &alloc = _Alloc())
        : _alloc(alloc) {
        _data = _alloc.allocate(_n);
        _cap = _size = _n;
        for ( std::size_t _i = 0; _i != _n; ++_i ) {
            std::construct_at(&_data[_i], 0);
        }
    }

    vector(std::size_t _n, const _Tp &val, const _Alloc &alloc = _Alloc())
        : _alloc(alloc) {
        _data = _alloc.allocate(_n);
        for ( std::size_t _i = 0; _i != _n; _i++ ) {
            std::construct_at(&_data[_i], val);
        }
    }

    // template
    // <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(std::random_access_iterator_tag,
    // _InputIt)>
    template <
        class _InputIt,
        typename = std::enable_if_t<std::is_convertible_v<
            typename std::iterator_traits<_InputIt>::iterator_category,
            std::random_access_iterator_tag>>>
    vector(_InputIt _first, _InputIt _last, const _Alloc &alloc = _Alloc())
        : _alloc(alloc) {
        std::size_t _n = _last - _first;
        _data = _alloc.allocate(_n);
        _cap = _size = _n;
        for ( std::size_t _i = 0; _i != _n; _i++ ) {
            std::construct_at(&_data[_i], *_first);
            ++_first;
        }
    }

    vector(vector &&_other) noexcept : _alloc(std::move(_other._alloc)) {
        _data = _other._data;
        _size = _other._size;
        _cap = _other._cap;
        _other._data = nullptr;
        _other._size = 0;
        _other._cap = 0;
    }

    vector(vector &&_other, const _Alloc &alloc) noexcept : _alloc(alloc) {
        _data = _other._data;
        _size = _other._size;
        _cap = _other._cap;
        _other._data = nullptr;
        _other._size = 0;
        _other._cap = 0;
    }

    vector &operator=(vector &&_other) noexcept {
        if ( &_other == this ) [[unlikely]] { return *this; }
        for ( std::size_t _i = 0; _i != _size; _i++ ) {
            std::destroy_at(&_data[_i]);
        }
        if ( _cap != 0 ) { _alloc.deallocate(_data, _cap); }
        _data = _other._data;
        _size = _other._size;
        _cap = _other._cap;
        _other._data = nullptr;
        _other._size = 0;
        _other._cap = 0;
        return *this;
    }

    vector(const vector &_other) : _alloc(_other._alloc) {
        _cap = _size = _other._size;
        if ( _size != 0 ) {
            _data = _alloc.allocate(_size);
            for ( std::size_t _i = 0; _i != _size; _i++ ) {
                std::construct_at(&_data[_i], std::as_const(_other._data[_i]));
            }
        } else {
            _data = nullptr;
        }
    }

    vector(const vector &_other, const _Alloc &alloc) : _alloc(alloc) {
        _cap = _size = _other._size;
        if ( _size != 0 ) {
            _data = _alloc.allocate(_size);
            for ( std::size_t _i = 0; _i != _size; _i++ ) {
                std::construct_at(&_data[_i], std::as_const(_other._data[_i]));
            }
        } else {
            _data = nullptr;
        }
    }

    vector &operator=(const vector &_other) {
        if ( &_other == this ) [[unlikely]] { return *this; }
        reserve(_other.size);
        _size = _other._size;
        for ( std::size_t _i = 0; _i != _size; _i++ ) {
            std::construct_at(&_data[_i], std::as_const(_other._data[_i]));
        }
        return *this;
    }

    vector &operator=(std::initializer_list<_Tp> _list) {
        assign(_list.begin(), _list.end());
        return *this;
    }

    void swap(vector &_other) noexcept {
        std::swap(_data, _other._data);
        std::swap(_size, _other._size);
        std::swap(_cap, _other._cap);
        std::swap(_alloc, _other._alloc);
    }

    void clear() noexcept {
        for ( std::size_t _i = 0; _i != _size; _i++ ) {
            std::destroy_at(&_data[_i]);
        }
        _size = 0;
    }

    void resize(std::size_t _n) {
        if ( _n < _size ) {
            for ( std::size_t _i = _n; _i != _size; _i++ ) {
                std::destroy_at(&_data[_i]);
            }
            _size = _n;
        } else if ( _n > _size ) {
            reserve(_n);
            for ( std::size_t _i = _size; _i != _n; _i++ ) {
                std::construct_at(&_data[_i]);
            }
        }
        _size = _n;
    }

    void resize(std::size_t _n, const _Tp &val) {
        if ( _n < _size ) {
            for ( std::size_t _i = _n; _i != _size; _i++ ) {
                std::destroy_at(&_data[_i]);
            }
            _size = _n;
        } else if ( _n > _size ) {
            reserve(_n);
            for ( std::size_t _i = _size; _i != _n; _i++ ) {
                std::construct_at(&_data[_i], val);
            }
        }
        _size = _n;
    }

    void shrink_to_fit() noexcept {
        auto _old_data = _data;
        auto _old_cap = _cap;
        _cap = _size;
        if ( _size == 0 ) {
            _data = nullptr;
        } else {
            _data = _alloc.allocate(_size);
        }

        if ( _old_cap != 0 ) [[likely]] {
            for ( std::size_t _i = 0; _i != _size; _i++ ) {
                std::construct_at(
                    &_data[_i], std::move_if_noexcept(_old_data[_i]));
                std::destroy_at(&_old_data[_i]);
            }
            _alloc.deallocate(_old_data, _old_cap);
        }
    }

    void reserve(std::size_t _n) { // 管理容量_cap，不应改变_size
        if ( _n < _cap ) { return; }
        _n = std::max(_n, _cap * 2);
        auto _old_data = _data;
        auto _old_cap = _cap;
        if ( _n == 0 ) {
            _data = nullptr;
            _cap = 0;
        } else {
            _data = _alloc.allocate(_n);
            _cap = _n;
        }
        if ( _old_cap != 0 ) {
            for ( std::size_t _i = 0; _i != _size; _i++ ) {
                std::construct_at(
                    &_data[_i], std::move_if_noexcept(_old_data[_i]));
            }
            for ( std::size_t _i = 0; _i != _size; _i++ ) {
                std::destroy_at(&_old_data[_i]);
            }
            _alloc.deallocate(_old_data, _old_cap);
        }
    }

    std::size_t capacity() const noexcept { return _cap; }

    std::size_t size() const noexcept { return _size; }

    bool empty() const noexcept { return _size == 0; }

    static constexpr std::size_t max_size() noexcept {
        return std::numeric_limits<std::size_t>::max() / sizeof(_Tp);
    }

    const _Tp &operator[](std::size_t _i) const noexcept { return _data[_i]; }

    _Tp &operator[](std::size_t _i) noexcept { return _data[_i]; }

    const _Tp &at(std::size_t _i) const {
        if ( _i >= _size ) [[unlikely]] {
            throw std::out_of_range("vector::at");
        }
        return _data[_i];
    }

    _Tp &at(std::size_t _i) {
        if ( _i >= _size ) [[unlikely]] {
            throw std::out_of_range("vector::at");
        }
        return _data[_i];
    }

    const _Tp &front() const noexcept { return *_data; }

    _Tp &front() noexcept { return *_data; }

    const _Tp &back() const noexcept { return _data[_size - 1]; }

    _Tp &back() noexcept { return _data[_size - 1]; }

    void push_back(const _Tp &val) {
        if ( _size + 1 >= _cap ) [[unlikely]] { reserve(_size + 1); }
        std::construct_at(&_data[_size], val);
        _size = _size + 1;
    }

    void push_back(_Tp &&val) {
        if ( _size + 1 >= _cap ) [[unlikely]] { reserve(_size + 1); }
        std::construct_at(&_data[_size], std::move(val));
        _size = _size + 1;
    }

    // 直接将构造对象的参数传进来
    template <typename... Args>
    _Tp &emplace_back(Args &&..._args) {
        if ( _size + 1 >= _cap ) [[unlikely]] { reserve(_size + 1); }
        _Tp *_p = &_data[_size];
        std::construct_at(_p, std::forward<Args>(_args)...);
        _size = _size + 1;
        return *_p;
    }

    _Tp *data() noexcept { return _data; }

    const _Tp *data() const noexcept { return _data; }

    const _Tp *cdata() const noexcept { return _data; }

    _Tp *begin() noexcept { return _data; }

    _Tp *end() noexcept { return _data + _size; }

    const _Tp *begin() const noexcept { return _data; }

    const _Tp *end() const noexcept { return _data + _size; }

    const _Tp *cbegin() const noexcept { return _data; }

    const _Tp *cend() const noexcept { return _data + _size; }

    std::reverse_iterator<_Tp *> rbegin() noexcept {
        return std::make_reverse_iterator(_data + _size);
    }

    std::reverse_iterator<_Tp *> rend() noexcept {
        return std::make_reverse_iterator(_data);
    }

    std::reverse_iterator<const _Tp *> rbegin() const noexcept {
        return std::make_reverse_iterator(_data + _size);
    }

    std::reverse_iterator<const _Tp *> rend() const noexcept {
        return std::make_reverse_iterator(_data);
    }

    std::reverse_iterator<const _Tp *> crbegin() const noexcept {
        return std::make_reverse_iterator(_data + _size);
    }

    std::reverse_iterator<const _Tp *> crend() const noexcept {
        return std::make_reverse_iterator(_data);
    }

    void pop_back() noexcept {
        _size -= 1;
        std::destroy_at(&_data[_size]);
    }

    _Tp *
    erase(const _Tp *_it) noexcept(std::is_nothrow_move_assignable_v<_Tp>) {
        std::size_t _i = _it - _data;
        for ( std::size_t _j = _i + 1; _j != _size; _j++ ) {
            _data[_j - 1] = std::move(_data[_j]);
        }
        _size -= 1;
        std::destroy_at(&_data[_size]);
        return const_cast<_Tp *>(_it);
    }

    // [_first, _last)
    _Tp *erase(const _Tp *_first, const _Tp *_last) noexcept(
        std::is_nothrow_move_assignable_v<_Tp>) {
        std::size_t diff = _last - _first;
        for ( std::size_t _j = _last - _data; _j != _size; _j++ ) {
            _data[_j - diff] = std::move(_data[_j]);
        }
        _size -= diff;
        for ( std::size_t _j = _size; _j != _size + diff; _j++ ) {
            std::destroy_at(&_data[_j]);
        }
        return const_cast<_Tp *>(_first);
    }

    void assign(std::size_t _n, const _Tp &val) {
        clear();
        reserve(_n);
        _size = _n;
        for ( std::size_t _i = 0; _i != _n; _i++ ) {
            std::construct_at(&_data[_i], val);
        }
    }

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(
        std::random_access_iterator, _InputIt)>
    void assign(_InputIt _first, _InputIt _last) {
        clear();
        std::size_t _n = _last - _first;
        reserve(_n);
        _size = _n;
        for ( std::size_t _i = 0; _i != _n; _i++ ) {
            std::construct_at(&_data[_i], *_first);
            ++_first;
        }
    }

    void assign(std::initializer_list<_Tp> _list) {
        assign(_list.begin(), _list.end());
    }

    template <typename... Args>
    _Tp *emplace(const _Tp *_it, Args &&...args) {
        std::size_t _j = _it - _data; // _j : insert index
        reserve(_size + 1);
        // shift backward
        for ( std::size_t _i = _size; _i != _j; _i-- ) {
            std::construct_at(&_data[_i], std::move(_data[_i - 1]));
            std::destroy_at(&_data[_i - 1]);
        }
        _size += 1;
        std::construct_at(&_data[_j], std::forward<Args>(args)...);
        return _data + _j;
    }

    _Tp *insert(const _Tp *_it, _Tp &&val) {
        std::size_t _j = _it - _data;
        reserve(_size + 1);
        for ( std::size_t _i = _size; _i != _j; _i-- ) {
            std::construct_at(&_data[_i], std::move(_data[_i - 1]));
            std::destroy_at(&_data[_i - 1]);
        }
        _size += 1;
        std::construct_at(&_data[_j], std::move(val));
        return _data + _j;
    }

    _Tp *insert(const _Tp *_it, const _Tp &val) {
        std::size_t _j = _it - _data;
        reserve(_size + 1);
        for ( std::size_t _i = _size; _i != _j; _i-- ) {
            std::construct_at(&_data[_i], std::move(_data[_i - 1]));
            std::destroy_at(&_data[_i - 1]);
        }
        _size += 1;
        std::construct_at(&_data[_j], val);
        return _data + _j;
    }

    _Tp *insert(const _Tp *_it, std::size_t _n, const _Tp &val) {
        std::size_t _j = _it - _data;
        if ( _n == 0 ) [[unlikely]] { return const_cast<_Tp *>(_it); }
        reserve(_size + _n);
        for ( std::size_t _i = _size; _i != _j; _i-- ) {
            std::construct_at(&_data[_i + _n - 1], std::move(_data[_i - 1]));
            std::destroy_at(&_data[_i - 1]);
        }
        _size += _n;
        for ( std::size_t _i = _j; _i != _j + _n; _i++ ) {
            std::construct_at(&_data[_i], val);
        }
        return _data + _j;
    }

    template <_LIBPENGCXX_REQUIRES_ITERATOR_CATEGORY(
        std::random_access_iterator, _InputIt)>
    _Tp *insert(const _Tp *_it, _InputIt _first, _InputIt _last) {
        std::size_t _j = _it - _data;
        std::size_t _n = _last - _first;
        if ( _n == 0 ) [[unlikely]] { return const_cast<_Tp *>(_it); }
        reserve(_size + _n);
        for ( std::size_t _i = _size; _i != _j; _i-- ) {
            std::construct_at(&_data[_i + _n - 1], std::move(_data[_i - 1]));
            std::destroy_at(&_data[_i - 1]);
        }
        _size += _n;
        for ( std::size_t _i = _j; _i != _j + _n; _i++ ) {
            std::construct_at(&_data[_i], *_first);
            ++_first;
        }
        return _data + _j;
    }

    _Tp *insert(const _Tp *_it, std::initializer_list<_Tp> _list) {
        return insert(_it, _list.begin(), _list.end());
    }

    ~vector() noexcept {
        for ( std::size_t _i = 0; _i != _size; _i++ ) {
            std::destroy_at(&_data[_i]);
        }
        if ( _cap != 0 ) { _alloc.deallocate(_data, _cap); }
    }

    _Alloc get_allocator() const noexcept { return _alloc; }

    _LIBPENGCXX_DEFINE_COMPARISON(vector);
};

} // namespace Marcus
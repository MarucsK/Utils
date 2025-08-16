#pragma once

#include <type_traits>
#include <utility>

namespace Marcus {

template <typename _Tp>
struct DefaultDeleter {
    void operator()(_Tp *p) const {
        delete p;
    }
};

template <typename _Tp>
struct DefaultDeleter<_Tp[]> {
    void operator()(_Tp *p) const {
        delete[] p;
    }
};

template <typename _Tp, typename _Deleter = DefaultDeleter<_Tp>>
struct unique_ptr {
private:
    _Tp *_M_p;
    [[no_unique_address]] _Deleter _M_deleter;

    template <typename _Up, typename _UDeleter>
    friend struct unique_ptr;

public:
    using element_type = _Tp;
    using pointer = _Tp *;
    using deleter_type = _Deleter;

    unique_ptr(std::nullptr_t = nullptr) noexcept : _M_p(nullptr) {}

    explicit unique_ptr(_Tp *p) noexcept : _M_p(p) {}

    template <typename _Up, typename _UDeleter>
        requires(std::convertible_to<_Up *, _Tp *>)
    unique_ptr(unique_ptr<_Up, _UDeleter> &&_other) noexcept
        : _M_p(_other._M_p) {
        _other._M_p = nullptr;
    }

    ~unique_ptr() noexcept {
        if (_M_p) {
            _M_deleter(_M_p);
        }
    }

    unique_ptr(const unique_ptr &_other) = delete;
    unique_ptr &operator=(const unique_ptr &__thta) = delete;

    unique_ptr(unique_ptr &&_other) noexcept : _M_p(_other._M_p) {
        _other._M_p = nullptr;
    }

    unique_ptr &operator=(unique_ptr &&_other) noexcept {
        if (this != &_other) [[likely]] {
            if (_M_p) {
                _M_deleter(_M_p);
            }
            _M_p = std::exchange(_other._M_p, nullptr);
        }
        return *this;
    }

    void swap(unique_ptr &_other) noexcept {
        std::swap(_M_p, _other._M_p);
    }

    _Tp *get() const noexcept {
        return _M_p;
    }

    _Tp *operator->() const noexcept {
        return _M_p;
    }

    std::add_lvalue_reference_t<_Tp> operator*() const noexcept {
        return *_M_p;
    }

    _Deleter get_deleter() const noexcept {
        return _M_deleter;
    }

    _Tp *release() noexcept {
        _Tp *__p = _M_p;
        _M_p = nullptr;
        return __p;
    }

    void reset(_Tp *__p = nullptr) noexcept {
        if (_M_p) {
            _M_deleter(_M_p);
        }
        _M_p = __p;
    }

    explicit operator bool() const noexcept {
        return _M_p != nullptr;
    }

    bool operator==(const unique_ptr &_other) const noexcept {
        return _M_p == _other._M_p;
    }

    bool operator!=(const unique_ptr &_other) const noexcept {
        return _M_p != _other._M_p;
    }

    bool operator<(const unique_ptr &_other) const noexcept {
        return _M_p < _other._M_p;
    }

    bool operator<=(const unique_ptr &_other) const noexcept {
        return _M_p <= _other._M_p;
    }

    bool operator>(const unique_ptr &_other) const noexcept {
        return _M_p > _other._M_p;
    }

    bool operator>=(const unique_ptr &_other) const noexcept {
        return _M_p >= _other._M_p;
    }
};

template <typename _Tp, typename _Deleter>
struct unique_ptr<_Tp[], _Deleter> : unique_ptr<_Tp, _Deleter> {
    using unique_ptr<_Tp, _Deleter>::unique_ptr;

    std::add_lvalue_reference_t<_Tp> operator[](std::size_t __i) {
        return this->get()[__i];
    }
};

// make_unique
// Creating a unique_ptr to an array with a known bound is not supported.

// non-array
template <typename _Tp, typename... _Args,
          std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
unique_ptr<_Tp> make_unique(_Args &&...__args) {
    return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

template <typename _Tp,
          std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
unique_ptr<_Tp>
make_unique_for_overwrite() { // C++20 create object, but not initialize.
    return unique_ptr<_Tp>(new _Tp);
}

// array with a unknown bound
template <typename _Tp,
          std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
unique_ptr<_Tp> make_unique(std::size_t __len) {
    return unique_ptr<_Tp>(new std::remove_extent_t<_Tp>[__len]());
}

template <typename _Tp,
          std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
unique_ptr<_Tp> make_unique_for_overwrite(std::size_t __len) {
    return unique_ptr<_Tp>(new std::remove_extent_t<_Tp>[__len]);
}

} // namespace Marcus

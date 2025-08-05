#pragma once

#include <memory/shared_ptr.hpp>
#include <utility>

namespace Marcus {

template <typename _Tp>
struct weak_ptr {
private:
    _Tp *_M_ptr;
    _SpCounter *_M_owner;

    template <typename>
    friend struct weak_ptr;

    template <typename>
    friend struct shared_ptr;

public:
    using element_type = _Tp;

    constexpr weak_ptr() noexcept : _M_ptr(nullptr), _M_owner(nullptr) {}

    weak_ptr(const weak_ptr &__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref_weak();
        }
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    weak_ptr(const weak_ptr<_Yp> &__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref_weak();
        }
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    weak_ptr(const shared_ptr<_Yp> &__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref_weak();
        }
    }

    weak_ptr(weak_ptr &&__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    weak_ptr(weak_ptr<_Yp> &&__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
    }

    ~weak_ptr() {
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
    }

    weak_ptr &operator=(const weak_ptr &__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (__other._M_owner) {
            __other._M_owner->_M_incref_weak();
        }
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        return *this;
    }

    template <typename _Yp>
    weak_ptr &operator=(const weak_ptr<_Yp> &__other) noexcept {
        if (__other._M_owner) {
            __other._M_owner->_M_incref_weak();
        }
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        return *this;
    }

    template <typename _Yp>
    weak_ptr &operator=(const shared_ptr<_Yp> &__other) noexcept {
        if (__other._M_owner) {
            __other._M_owner->_M_incref_weak();
        }
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        return *this;
    }

    weak_ptr &operator=(weak_ptr &&__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
        return *this;
    }

    template <typename _Yp>
    weak_ptr &operator=(weak_ptr<_Yp> &&__other) noexcept {
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
        return *this;
    }

    void reset() noexcept {
        if (_M_owner) {
            _M_owner->_M_decref_weak();
        }
        _M_ptr = nullptr;
        _M_owner = nullptr;
    }

    void swap(weak_ptr &__other) noexcept {
        std::swap(_M_ptr, __other._M_ptr);
        std::swap(_M_owner, __other._M_owner);
    }

    long use_count() const noexcept {
        return _M_owner ? _M_owner->_M_cntref() : 0;
    }

    bool expired() const noexcept {
        return use_count() == 0;
    }

    shared_ptr<_Tp> lock() const noexcept {
        if (_M_owner && _M_owner->_M_try_lock()) {
            return shared_ptr<_Tp>(_M_ptr, _M_owner);
        }
        return shared_ptr<_Tp>();
    }

    template <typename _Yp>
    bool owner_before(const weak_ptr<_Yp> &__other) const noexcept {
        return _M_owner < __other._M_owner;
    }

    template <typename _Yp>
    bool owner_before(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_owner < __other._M_owner;
    }
};

} // namespace Marcus

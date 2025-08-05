#pragma once

#include <algorithm>
#include <atomic>
#include <memory/unique_ptr.hpp>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace Marcus {

template <typename>
struct weak_ptr;

struct _SpCounter {
    std::atomic<long> _M_refcnt;
    std::atomic<long> _M_weak_refcnt;

    _SpCounter() noexcept : _M_refcnt(1), _M_weak_refcnt(1) {}

    _SpCounter(_SpCounter &&) = delete;

    void _M_incref() noexcept {
        _M_refcnt.fetch_add(1, std::memory_order_relaxed);
    }

    void _M_decref() noexcept {
        if (_M_refcnt.fetch_sub(1, std::memory_order_relaxed) == 1) {
            _M_destroy();
            _M_decref_weak();
        }
    }

    void _M_incref_weak() noexcept {
        _M_weak_refcnt.fetch_add(1, std::memory_order_relaxed);
    }

    void _M_decref_weak() noexcept {
        if (_M_weak_refcnt.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

    long _M_cntref() const noexcept {
        return _M_refcnt.load(std::memory_order_relaxed);
    }

    long _M_cntweakref() const noexcept {
        return _M_weak_refcnt.load(std::memory_order_relaxed);
    }

    bool _M_try_lock() noexcept {
        long __count = _M_refcnt.load(std::memory_order_relaxed);
        while (__count != 0) {
            if (_M_refcnt.compare_exchange_weak(__count, __count + 1,
                                                std::memory_order_acq_rel,
                                                std::memory_order_relaxed)) {
                return true;
            }
        }
        return false;
    }

    virtual void _M_destroy() noexcept = 0;

    virtual ~_SpCounter() = default;
};

// 管理指向对象的指针和删除器
template <typename _Tp, typename _Deleter>
struct _SpCounterImpl final : _SpCounter {
    _Tp *_M_ptr;
    [[no_unique_address]] _Deleter _M_deleter;

    explicit _SpCounterImpl(_Tp *__ptr) noexcept : _M_ptr(__ptr) {}

    explicit _SpCounterImpl(_Tp *__ptr, _Deleter __deleter) noexcept
        : _M_ptr(__ptr),
          _M_deleter(std::move(__deleter)) {}

    void _M_destroy() noexcept override {
        _M_deleter(_M_ptr);
    }
};

// 将控制块和对象分配在同一块内存
template <typename _Tp, typename _Deleter>
struct _SpCounterImplFused final : _SpCounter {
    _Tp *_M_ptr;
    void *_M_mem; // 指向分配的原始内存块的指针
    [[no_unique_address]] _Deleter _M_deleter;

    explicit _SpCounterImplFused(_Tp *__ptr, void *__mem,
                                 _Deleter __deleter) noexcept
        : _M_ptr(__ptr),
          _M_mem(__mem),
          _M_deleter(std::move(__deleter)) {}

    void _M_destroy() noexcept override {
        _M_deleter(_M_ptr);
    }

    void operator delete(void *__mem) noexcept {
#if __cpp_aligned_new
        ::operator delete(
            __mem, std::align_val_t(
                       std::max(alignof(_Tp), alignof(_SpCounterImplFused))));
#else
        ::operator delete(__mem);
#endif
    }
};

template <typename _Tp>
struct shared_ptr {
private:
    _Tp *_M_ptr;
    _SpCounter *_M_owner;

    template <typename>
    friend struct shared_ptr;

    template <typename>
    friend struct weak_ptr;

    explicit shared_ptr(_Tp *__ptr, _SpCounter *__owner) noexcept
        : _M_ptr(__ptr),
          _M_owner(__owner) {}

public:
    using element_type = _Tp;
    using pointer = _Tp *;

    shared_ptr(std::nullptr_t = nullptr) noexcept : _M_owner(nullptr) {}

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    explicit shared_ptr(_Yp *__ptr)
        : _M_ptr(__ptr),
          _M_owner(new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr)) {
        _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
    }

    template <typename _Yp, typename _Deleter,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    explicit shared_ptr(_Yp *__ptr, _Deleter __deleter)
        : _M_ptr(__ptr),
          _M_owner(
              new _SpCounterImpl<_Yp, _Deleter>(__ptr, std::move(__deleter))) {
        _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
    }

    template <typename _Yp, typename _Deleter,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    explicit shared_ptr(Marcus::unique_ptr<_Yp, _Deleter> &&__ptr)
        : shared_ptr(__ptr.release(), __ptr.get_deleter()) {}

    template <class _Yp>
    inline friend shared_ptr<_Yp>
    _S_makeSharedFused(_Yp *__ptr, _SpCounter *__owner) noexcept;

    shared_ptr(const shared_ptr &__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref();
        }
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    shared_ptr(const shared_ptr<_Yp> &__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref();
        }
    }

    shared_ptr(shared_ptr &&__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    shared_ptr(shared_ptr<_Yp> &&__other) noexcept
        : _M_ptr(__other._M_ptr),
          _M_owner(__other._M_owner) {
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
    }

    template <typename _Yp>
    shared_ptr(const shared_ptr<_Yp> &__other, _Tp *__ptr) noexcept
        : _M_ptr(__ptr),
          _M_owner(__other._M_owner) {
        if (_M_owner) {
            _M_owner->_M_incref();
        }
    }

    template <typename _Yp>
    shared_ptr(const shared_ptr<_Yp> &&__other, _Tp *__ptr) noexcept
        : _M_ptr(__ptr),
          _M_owner(__other._M_owner) {
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
    }

    shared_ptr &operator=(const shared_ptr &__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        if (_M_owner) {
            _M_owner->_M_incref();
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
        return *this;
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    shared_ptr &operator=(const shared_ptr<_Yp> &__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        if (_M_owner) {
            _M_owner->_M_incref();
        }
        return *this;
    }

    template <typename _Yp,
              std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
    shared_ptr &operator=(shared_ptr<_Yp> &&__other) noexcept {
        if (this == &__other) {
            return *this;
        }
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = __other._M_ptr;
        _M_owner = __other._M_owner;
        __other._M_ptr = nullptr;
        __other._M_owner = nullptr;
        return *this;
    }

    void reset() noexcept {
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_owner = nullptr;
        _M_ptr = nullptr;
    }

    template <typename _Yp>
    void reset(_Yp *__ptr) {
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = nullptr;
        _M_owner = nullptr;
        _M_ptr = __ptr;
        _M_owner = new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr);
        _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
    }

    template <typename _Yp, typename _Deleter>
    void reset(_Yp *__ptr, _Deleter __deleter) {
        if (_M_owner) {
            _M_owner->_M_decref();
        }
        _M_ptr = nullptr;
        _M_owner = nullptr;
        _M_ptr = __ptr;
        _M_owner =
            new _SpCounterImpl<_Yp, _Deleter>(__ptr, std::move(__deleter));
        _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
    }

    ~shared_ptr() noexcept {
        if (_M_owner) {
            _M_owner->_M_decref();
        }
    }

    long use_count() noexcept {
        return _M_owner ? _M_owner->_M_cntref() : 0;
    }

    bool unique() noexcept {
        return _M_owner ? _M_owner->_M_cntref() == 1 : true;
    }

    template <typename _Yp>
    bool operator==(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr == __other._M_ptr;
    }

    template <typename _Yp>
    bool operator!=(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr != __other._M_ptr;
    }

    template <typename _Yp>
    bool operator<(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr < __other._M_ptr;
    }

    template <typename _Yp>
    bool operator<=(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr <= __other._M_ptr;
    }

    template <typename _Yp>
    bool operator>(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr > __other._M_ptr;
    }

    template <typename _Yp>
    bool operator>=(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_ptr >= __other._M_ptr;
    }

    template <typename _Yp>
    bool owner_before(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_owner < __other._M_owner;
    }

    template <typename _Yp>
    bool owner_equal(const shared_ptr<_Yp> &__other) const noexcept {
        return _M_owner == __other._M_owner;
    }

    void swap(shared_ptr &__other) noexcept {
        std::swap(_M_ptr, __other._M_ptr);
        std::swap(_M_owner, __other._M_owner);
    }

    _Tp *get() const noexcept {
        return _M_ptr;
    }

    _Tp *operator->() const noexcept {
        return _M_ptr;
    }

    std::add_lvalue_reference_t<_Tp> operator*() const noexcept {
        return *_M_ptr;
    }

    explicit operator bool() const noexcept {
        return _M_ptr != nullptr;
    }
};

template <typename _Tp>
inline shared_ptr<_Tp> _S_makeSharedFused(_Tp *__ptr,
                                          _SpCounter *__owner) noexcept {
    return shared_ptr<_Tp>(__ptr, __owner);
}

template <typename _Tp>
struct shared_ptr<_Tp[]> : shared_ptr<_Tp> {
    using shared_ptr<_Tp>::shared_ptr;

    std::add_lvalue_reference_t<_Tp> operator[](std::size_t __i) {
        return this->get()[__i];
    }
};

template <typename _Tp>
struct enable_shared_from_this {
private:
    _SpCounter *_M_owner;

protected:
    enable_shared_from_this() noexcept : _M_owner(nullptr) {}

    shared_ptr<_Tp> shared_from_this() {
        static_assert(std::is_base_of_v<enable_shared_from_this, _Tp>,
                      "must be derived class");
        if (!_M_owner) {
            throw std::bad_weak_ptr();
        }
        _M_owner->_M_incref();
        return _S_makeSharedFused(static_cast<_Tp *>(this), _M_owner);
    }

    shared_ptr<const _Tp> shared_from_this() const {
        static_assert(std::is_base_of_v<enable_shared_from_this, _Tp>,
                      "must be derived class");
        if (!_M_owner) {
            throw std::bad_weak_ptr();
        }
        _M_owner->_M_incref();
        return _S_makeSharedFused(static_cast<const _Tp *>(this), _M_owner);
    }

    template <typename _Up>
    inline friend void
    _S_setEnableSharedFromThisOwner(enable_shared_from_this<_Up> *__ptr,
                                    _SpCounter *__owner);
};

template <typename _Up>
inline void _S_setEnableSharedFromThisOwner(enable_shared_from_this<_Up> *__ptr,
                                            _SpCounter *__owner) {
    __ptr->_M_owner = __owner;
}

template <typename _Tp,
          std::enable_if_t<std::is_base_of_v<enable_shared_from_this<_Tp>, _Tp>,
                           int> = 0>
void _S_setupEnableSharedFromThis(_Tp *__ptr, _SpCounter *__owner) {
    _S_setEnableSharedFromThisOwner(
        static_cast<enable_shared_from_this<_Tp> *>(__ptr), __owner);
}

template <typename _Tp,
          std::enable_if_t<
              !std::is_base_of_v<enable_shared_from_this<_Tp>, _Tp>, int> = 0>
void _S_setupEnableSharedFromThis(_Tp *, _SpCounter *) {}

template <typename _Tp, typename... _Args,
          std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> =
              0> // _Tp不是未知边界数组
shared_ptr<_Tp> make_shared(_Args... __args) {
    const auto __deleter = [](_Tp *__ptr) noexcept {
        __ptr->~_Tp();
    };
    using _Counter = _SpCounterImplFused<_Tp, decltype(__deleter)>;
    constexpr std::size_t __offset = std::max(alignof(_Tp), sizeof(_Counter));
    constexpr std::size_t __align = std::max(alignof(_Tp), alignof(_Counter));
    constexpr std::size_t __size = __offset + sizeof(_Tp);
#if __cpp_aligned_new
    void *__mem = ::operator new(__size, std::align_val_t(__align));
    _Counter *__counter = reinterpret_cast<_Counter *>(
        __mem); // 将分配的内存起始地址解释为控制块指针
#else
    void *__mem = ::operator new(
        __size +
        __align); // 不支持对齐new则分配要比实际大一点。分配额外内存用于对齐
    _Counter *__counter = reinterpret_cast<_Counter *>(
        reinterpret_cast<std::size_t>(__mem) & __align);
#endif
    _Tp *__object =
        reinterpret_cast<_Tp *>(reinterpret_cast<char *>(__counter) + __offset);
    try {
        new (__object) _Tp(std::forward<_Args>(__args)...);
    } catch (...) {
#if __cpp_aligned_new
        ::operator delete(__mem, std::align_val_t(__align));
#else
        ::operator delete(__mem);
#endif
        throw;
    }
    new (__counter) _Counter(__object, __mem, __deleter);
    _S_setupEnableSharedFromThis(__object, __counter);
    return _S_makeSharedFused(__object, __counter);
}

template <typename _Tp,
          std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
shared_ptr<_Tp> make_shared_for_overwrite() {
    const auto __deleter = [](_Tp *__ptr) noexcept {
        __ptr->~_Tp();
    };
    using _Counter = _SpCounterImplFused<_Tp, decltype(__deleter)>;
    constexpr std::size_t __offset = std::max(alignof(_Tp), sizeof(_Counter));
    constexpr std::size_t __align = std::max(alignof(_Tp), alignof(_Counter));
    constexpr std::size_t __size = __offset + sizeof(_Tp);
#if __cpp_aligned_new
    void *__mem = ::operator new(__size, std::align_val_t(__align));
    _Counter *__counter = reinterpret_cast<_Counter *>(__mem);
#else
    void *__mem = ::operator new(__size + __align);
    _Counter *__counter = reinterpret_cast<_Counter *>(
        reinterpret_cast<std::size_t>(__mem) & __align);
#endif
    _Tp *__object =
        reinterpret_cast<_Tp *>(reinterpret_cast<char *>(__counter) + __offset);
    try {
        new (__object) _Tp;
    } catch (...) {
#if __cpp_aligned_new
        ::operator delete(__mem, std::align_val_t(__align));
#else
        ::operator delete(__mem);
#endif
        throw;
    }
    new (__counter) _Counter(__object, __mem, __deleter);
    _S_setupEnableSharedFromThis(__object, __counter);
    return _S_makeSharedFused(__object, __counter);
}

template <typename _Tp, typename... _Args,
          std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
shared_ptr<_Tp> make_shared(std::size_t __len) {
    std::remove_extent_t<_Tp> *__p = new std::remove_extent_t<_Tp>[__len];
    try {
        return shared_ptr<_Tp>(__p);
    } catch (...) {
        delete[] __p;
        throw;
    }
}

template <typename _Tp,
          std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
shared_ptr<_Tp> make_shared_for_overwrite(std::size_t __len) {
    std::remove_extent_t<_Tp> *__p = new std::remove_extent_t<_Tp>[__len];
    try {
        return shared_ptr<_Tp>(__p);
    } catch (...) {
        delete[] __p;
        throw;
    }
}

template <class _Tp>
bool operator==(const shared_ptr<_Tp> &__a, std::nullptr_t) noexcept {
    return !__a;
}

template <class _Tp>
bool operator==(std::nullptr_t, const shared_ptr<_Tp> &__a) noexcept {
    return !__a;
}

template <class _Tp>
bool operator!=(const shared_ptr<_Tp> &__a, std::nullptr_t) noexcept {
    return (bool)__a;
}

template <class _Tp>
bool operator!=(std::nullptr_t, const shared_ptr<_Tp> &__a) noexcept {
    return (bool)__a;
}

// C++ 17
template <typename _Tp, typename _Up>
shared_ptr<_Tp> static_pointer_cast(const shared_ptr<_Up> &__ptr) {
    return shared_ptr<_Tp>(__ptr, static_cast<_Tp *>(__ptr.get()));
}

template <typename _Tp, typename _Up>
shared_ptr<_Tp> const_pointer_cast(const shared_ptr<_Up> &__ptr) {
    return shared_ptr<_Tp>(__ptr, const_cast<_Tp *>(__ptr.get()));
}

template <typename _Tp, typename _Up>
shared_ptr<_Tp> reinterpret_pointer_cast(const shared_ptr<_Up> &__ptr) {
    return shared_ptr<_Tp>(__ptr, reinterpret_cast<_Tp *>(__ptr.get()));
}

template <typename _Tp, typename _Up>
shared_ptr<_Tp> dynamic_pointer_cast(const shared_ptr<_Up> &__ptr) {
    _Tp *__p = dynamic_cast<_Tp *>(__ptr.get());
    if (__p) {
        return shared_ptr<_Tp>(__ptr, __p);
    } else {
        return nullptr;
    }
}
} // namespace Marcus

#pragma once

#include <exception>
#include <initializer_list>
#include <type_traits>

namespace Marcus {

struct BadOptionalAccess : std::exception {
    BadOptionalAccess() = default;
    virtual ~BadOptionalAccess() = default;

    const char *what() const noexcept override {
        return "BadOptionalAccess";
    }
};

struct Nullopt {
    explicit Nullopt() = default;
};

constexpr Nullopt nullopt;

struct InPlace {
    explicit InPlace() = default;
};

constexpr InPlace inPlace;

template <typename T>
struct optional {
private:
    bool _has_value;

    union {
        T _value;
    };

public:
    optional(T &&value) noexcept : _has_value(true), _value(std::move(value)) {}

    optional(const T &val) noexcept
        : _has_value(true),
          _value(std::move(value)) {}

    optional() noexcept : _has_value(false) {}

    optional(Nullopt) noexcept : _has_value(false) {}

    template <typename... Ts>
    explicit optional(InPlace, Ts &&...value_args)
        : _has_value(true),
          _value(std::forward<Ts>(value_args)...) {}

    template <typename U, typename... Ts>
    explicit optional(InPlace, std::initializer_list<U> ilist,
                      Ts &&...value_args)
        : _has_value(true),
          _value(ilist, std::forward<Ts>(value_args)...) {}

    optional(const optional &other) : _has_value(other._has_value) {
        if (_has_value) {
            new (&_value) T(other._value); // placement-new: only construct
        }
    }

    optional(optional &&other) noexcept : _has_value(other._has_value) {
        if (_has_value) {
            new (&_value) T(std::move(other._value));
        }
    }

    optional &operator=(Nullopt) noexcept {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        return *this;
    }

    optional &operator=(T &&value) noexcept {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        new (&_value) T(std::move(value)); // 调用T的移动构造
        _has_value = true;
        return *this;
    }

    optional &operator=(const T &value) noexcept {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        new (&_value) T(value);
        _has_value = true;
        return *this;
    }

    optional &operator=(const optional &other) {
        if (this == &other) {
            return *this;
        }
        if (_has_value) {
            _value.T();
            _has_value = false;
        }
        if (other._has_value) {
            new (&_value) T(other._value);
        }
        _has_value = other._has_value;
        return *this;
    }

    optional &operator=(optional &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        if (other._has_value) {
            new (&_value) T(std::move(other._value));
            other._value.~T();
        }
        _has_value = other._has_value;
        other._has_value = false;
        return *this;
    }

    template <typename... Ts>
    void emplace(Ts &&...value_args) {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        new (&_value) T(std::forward<Ts>(value_args)...);
        _has_value = true;
    }

    template <typename U, typename... Ts>
    void emplace(std::initializer_list<U> ilist, Ts &&...value_args) {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
        new (&_value) T(ilist, std::forward<Ts>(value_args)...);
        _has_value = true;
    }

    void reset() noexcept {
        if (_has_value) {
            _value.~T();
            _has_value = false;
        }
    }

    ~optional() noexcept {
        if (_has_value) {
            _value.~T(); // only destruct, not deallocate memory
        }
    }

    bool has_value() const noexcept {
        return _has_value;
    }

    explicit operator bool() const noexcept {
        return _has_value;
    }

    bool operator==(Nullopt) const noexcept {
        return !_has_value;
    }

    friend bool operator==(Nullopt, const optional &self) noexcept {
        return !self._has_value;
    }

    bool operator!=(Nullopt) const noexcept {
        return _has_value;
    }

    friend bool operator!=(Nullopt, const optional &self) noexcept {
        return self._has_value;
    }

    const T &value() const & {
        if (!_has_value) {
            throw BadOptionalAccess();
        }
        return _value;
    }

    T &value() & {
        if (!_has_value) {
            throw BadOptionalAccess();
        }
        return _value;
    }

    const T &&value() const && {
        if (!_has_value) {
            throw BadOptionalAccess();
        }
        return std::move(_value);
    }

    T &&value() && {
        if (!_has_value) {
            throw BadOptionalAccess();
        }
        return std::move(_value);
    }

    const T &operator*() const & noexcept {
        return _value;
    }

    T &operator*() & noexcept {
        return _value;
    }

    const T &&operator*() const && noexcept {
        return std::move(_value);
    }

    T &&operator&() && noexcept {
        return std::move(_value);
    }

    const T *operator->() const noexcept {
        return &_value;
    }

    T *operator->() noexcept {
        return &_value;
    }

    T value_or(T default_value) const & {
        if (!_has_value) {
            return default_value;
        }
        return _value;
    }

    T value_or(T default_value) && noexcept {
        if (!_has_value) {
            return default_value;
        }
        return std::move(_value);
    }

    bool operator==(const optional<T> &other) const noexcept {
        if (_has_value != other._has_value) {
            return false;
        }
        if (_has_value) {
            return _value == other._value;
        }
        return true;
    }

    bool operator!=(const optional<T> &other) const noexcept {
        if (_has_value != other._has_value) {
            return true;
        }
        if (_has_value) {
            return _value != other._value;
        }
        return false;
    }

    bool operator>(const optional &other) const noexcept {
        if (!_has_value || !other._has_value) {
            return false;
        }
        return _value > other._value;
    }

    bool operator<(const optional &other) const noexcept {
        if (!_has_value || !other._has_value) {
            return false;
        }
        return _value < other._value;
    }

    bool operator>=(const optional &other) const noexcept {
        if (!_has_value || !other._has_value) {
            return true;
        }
        return _value >= other._value;
    }

    bool operator<=(const optional &other) const noexcept {
        if (!_has_value || !other._has_value) {
            return true;
        }
        return _value <= other._value;
    }

    template <typename F>
    auto and_then(F &&f) const
        & -> std::remove_cv_t<std::remove_reference_t<decltype(f(_value))>> {
        if (_has_value) {
            return std::forward<F>(f)(_value);
        } else {
            return std::remove_cv_t<
                std::remove_reference_t<decltype(f(_value))>>{};
        }
    }

    template <typename F>
    auto and_then(F &&f) & -> typename std::remove_cv_t<
        typename std::remove_reference_t<decltype(f(_value))>> {
        if (_has_value) {
            return std::forward<F>(f)(_value);
        } else {
            return typename std::remove_cv_t<
                typename std::remove_reference_t<decltype(f(_value))>>{};
        }
    }

    template <typename F>
    auto and_then(F &&f) const && -> std::remove_cv_t<
        std::remove_reference_t<decltype(f(std::move(_value)))>> {
        if (_has_value) {
            return std::forward<F>(f)(std::move(_value));
        } else {
            return std::remove_cv_t<
                std::remove_reference_t<decltype(f(std::move(_value)))>>{};
        }
    }

    template <typename F>
    auto and_then(F &&f) && -> std::remove_cv_t<
        std::remove_reference_t<decltype(f(std::move(_value)))>> {
        if (_has_value) {
            return std::forward<F>(f)(std::move(_value));
        } else {
            return std::remove_cv_t<
                std::remove_reference_t<decltype(f(std::move(_value)))>>{};
        }
    }

    template <typename F>
    auto transform(F &&f) const & -> optional<
        std::remove_cv_t<std::remove_reference_t<decltype(f(_value))>>> {
        if (_has_value) {
            return std::forward<F>(f)(_value);
        } else {
            return nullopt;
        }
    }

    template <typename F>
    auto transform(F &&f) & -> optional<
        std::remove_cv_t<std::remove_reference_t<decltype(f(_value))>>> {
        if (_has_value) {
            return std::forward<F>(f)(_value);
        } else {
            return nullopt;
        }
    }

    template <typename F>
    auto transform(F &&f) const && -> optional<std::remove_cv_t<
        std::remove_reference_t<decltype(f(std::move(_value)))>>> {
        if (_has_value) {
            return std::forward<F>(f)(std::move(_value));
        } else {
            return nullopt;
        }
    }

    template <typename F>
    auto transform(F &&f) && -> optional<std::remove_cv_t<
        std::remove_reference_t<decltype(f(std::move(_value)))>>> {
        if (_has_value) {
            return std::forward<F>(f)(std::move(_value));
        } else {
            return nullopt;
        }
    }

    template <class F, typename std::enable_if<
                           std::is_copy_constructible<T>::value, int>::type = 0>
    optional or_else(F &&f) const & {
        if (_has_value) {
            return *this;
        } else {
            return std::forward<F>(f)();
        }
    }

    template <class F, typename std::enable_if<
                           std::is_move_constructible<T>::value, int>::type = 0>
    optional or_else(F &&f) && {
        if (_has_value) {
            return std::move(*this);
        } else {
            return std::forward<F>(f)();
        }
    }

    void swap(optional &other) noexcept {
        if (_has_value && other._has_value) {
            std::swap(_value, other._value);
        } else if (!_has_value && !other._has_value) {
        } else if (_has_value) {
            other.emplace(std::move(_value));
        } else {
            emplace(std::move(other._value));
            other.reset();
        }
    }
};

#if _cpp_deduction_guides
template <typename T>
optional(T) -> optional<T>;
#endif

template <typename T>
optional<T> make_optional(T value) {
    return optional<T>(std::move(value));
}

} // namespace Marcus

#pragma once

#include <exception>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace Marcus {

struct BadAnyCast : std::exception {
    BadAnyCast() = default;
    virtual ~BadAnyCast() = default;

    const char *what() const noexcept override {
        return "BadAnyCast";
    }
};

template <typename T>
struct InPlaceType {
    explicit InPlaceType() = default;
};

template <typename T>
constexpr InPlaceType<T> in_place_type{};

struct AnyConcept {
    virtual ~AnyConcept() = default;
    virtual AnyConcept *clone() const = 0;
    virtual AnyConcept *move_clone() = 0;
    virtual const std::type_info &type() const noexcept = 0;
    virtual void *get_ptr() noexcept = 0;
    virtual const void *get_ptr() const noexcept = 0;
};

template <typename T>
struct AnyModel : AnyConcept {
    T _value;

    template <typename... Args>
    AnyModel(Args &&...args) : _value(std::forward<Args>(args)...) {}

    AnyConcept *clone() const override {
        return new AnyModel<T>(
            _value); // Create a new AnyModel with a copy of _value
    }

    AnyConcept *move_clone() override {
        return new AnyModel<T>(
            std::move(_value)); // Create a new AnyModel with a moved _value
    }

    const std::type_info &type() const noexcept override {
        return typeid(T); // Return type_info of T
    }

    void *get_ptr() noexcept override {
        return &_value; // Return pointer to the stored value
    }

    const void *get_ptr() const noexcept override {
        return &_value; // Return const pointer to the stored value
    }
};

class any {
private:
    AnyConcept *_storage;

    void *_get_value_ptr() noexcept {
        return _storage ? _storage->get_ptr() : nullptr;
    }

    const void *_get_value_ptr() const noexcept {
        return _storage ? _storage->get_ptr() : nullptr;
    }

public:
    any() noexcept : _storage(nullptr) {}

    template <typename T, typename = std::enable_if_t<
                              !std::is_same_v<std::decay_t<T>, any>>>
    any(T &&value)
        : _storage(new AnyModel<std::decay_t<T>>(std::forward<T>(value))) {}

    any(const any &other)
        : _storage(other._storage ? other._storage->clone() : nullptr) {}

    any(any &&other) noexcept : _storage(other._storage) {
        other._storage = nullptr;
    }

    template <typename T, typename... Args>
    explicit any(InPlaceType<T>, Args &&...args)
        : _storage(new AnyModel<T>(std::forward<Args>(args)...)) {}

    template <typename T, typename U, typename... Args>
    explicit any(InPlaceType<T>, std::initializer_list<U> ilist, Args &&...args)
        : _storage(new AnyModel<T>(ilist, std::forward<Args>(args)...)) {}

    ~any() noexcept {
        delete _storage;
    }

    any &operator=(const any &other) {
        any(other).swap(*this);
        return *this;
    }

    any &operator=(any &&other) noexcept {
        any(std::move(other)).swap(*this);
        return *this;
    }

    template <typename T>
    any &operator=(T &&value) {
        any(std::forward<T>(value)).swap(*this);
        return *this;
    }

    template <typename T, typename... Args>
    void emplace(Args &&...args) {
        reset();
        _storage = new AnyModel<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename U, typename... Args>
    void emplace(std::initializer_list<U> ilist, Args &&...args) {
        reset();
        _storage = new AnyModel<T>(ilist, std::forward<Args>(args)...);
    }

    void reset() noexcept {
        delete _storage;
        _storage = nullptr;
    }

    void swap(any &other) noexcept {
        using std::swap;
        swap(_storage, other._storage);
    }

    bool has_value() const noexcept {
        return _storage != nullptr;
    }

    const std::type_info &type() const noexcept {
        return _storage ? _storage->type() : typeid(void);
    }

    friend void *any_cast_impl_ref(any *operand, const std::type_info &info);
    friend const void *any_cast_impl_ref(const any *operand,
                                         const std::type_info &info);
    template <typename T>
    friend T any_cast(any &operand);

    template <typename T>
    friend T any_cast(const any &operand);

    template <typename T>
    friend T any_cast(any &&operand);

    template <typename T>
    friend const T *any_cast(const any *operand) noexcept;

    template <typename T>
    friend T *any_cast(any *operand) noexcept;
};

inline void *any_cast_impl_ref(any *operand, const std::type_info &info) {
    if (!operand || !operand->has_value() || operand->type() != info) {
        throw BadAnyCast();
    }
    return operand->_get_value_ptr();
}

inline const void *any_cast_impl_ref(const any *operand,
                                     const std::type_info &info) {
    if (!operand || !operand->has_value() || operand->type() != info) {
        throw BadAnyCast();
    }
    return operand->_get_value_ptr();
}

template <typename T>
T any_cast(any &operand) {
    using U = std::remove_reference_t<T>;
    void *p = any_cast_impl_ref(&operand, typeid(U));
    return static_cast<T>(*static_cast<U *>(p));
}

template <typename T>
T any_cast(const any &operand) {
    using U = std::remove_reference_t<T>;
    const void *p = any_cast_impl_ref(&operand, typeid(U));
    return static_cast<T>(*static_cast<const U *>(p));
}

template <typename T>
T any_cast(any &&operand) {
    using U = std::remove_reference_t<T>;
    void *p = any_cast_impl_ref(&operand, typeid(U));
    return static_cast<T>(std::move(*static_cast<U *>(p)));
}

template <typename T>
const T *any_cast(const any *operand) noexcept {
    if (!operand || !operand->has_value() || operand->type() != typeid(T)) {
        return nullptr;
    }
    return static_cast<const T *>(operand->_get_value_ptr());
}

template <typename T>
T *any_cast(any *operand) noexcept {
    if (!operand || !operand->has_value() || operand->type() != typeid(T)) {
        return nullptr;
    }
    return static_cast<T *>(operand->_get_value_ptr());
}

template <typename T, typename... Args>
any make_any(Args &&...args) {
    return any(in_place_type<T>, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
any make_any(std::initializer_list<U> il, Args &&...args) {
    return any(in_place_type<T>, il, std::forward<Args>(args)...);
}

} // namespace Marcus

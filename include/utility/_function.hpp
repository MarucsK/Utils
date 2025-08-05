#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace Marcus {

template <class _FnSig>
struct Function {
    static_assert(!std::is_same_v<_FnSig, _FnSig>,
                  "not a valid function signature");
};

// type : _Ret(_Args...)
template <class _Ret, class... _Args>
struct Function<_Ret(_Args...)> {
private:
    struct _FuncBase {
        virtual _Ret _M_call(_Args... __args) = 0; // 类型擦除后的统一接口
        virtual std::unique_ptr<_FuncBase>
        _M_clone() const = 0;                      // 克隆底层可调用对象
        virtual const std::type_info &_M_type() const = 0;
        virtual ~_FuncBase() = default;
    };

    template <class _Fn>
    struct _FuncImpl : _FuncBase {
        _Fn _M_f;

        template <class... _CArgs>
        explicit _FuncImpl(std::in_place_t, _CArgs &&...__args)
            : _M_f(std::forward<_CArgs>(__args)...) {}

        _Ret _M_call(_Args... __args) override {
            return std::invoke(_M_f, std::forward<_Args>(__args)...);
        }

        std::unique_ptr<_FuncBase> _M_clone() const override {
            return std::make_unique<_FuncImpl>(std::in_place, _M_f);
        }

        const std::type_info &_M_type() const override {
            return typeid(_Fn);
        }
    };

    std::unique_ptr<_FuncBase> _M_base;

public:
    Function() = default; // _M_base 初始化为 nullptr

    Function(std::nullptr_t) noexcept : Function() {}

    // 从任意的可调用对象_Fn 构造Function对象
    // enable_if_t 的作用：阻止 Function 从不可调用的对象中初始化
    // 另外标准要求 Function 还需要函数对象额外支持拷贝（用于 _M_clone）
    template <class _Fn,
              class = std::enable_if_t<
                  std::is_invocable_r_v<_Ret, std::decay_t<_Fn>, _Args...> &&
                  std::is_copy_constructible_v<_Fn> &&
                  !std::is_same_v<std::decay_t<_Fn>, Function<_Ret(_Args...)>>>>
    Function(_Fn &&__f) // 没有 explicit，允许 lambda 表达式隐式转换成 Function
        : _M_base(std::make_unique<_FuncImpl<std::decay_t<_Fn>>>(
              std::in_place, std::forward<_Fn>(__f))) {}

    Function(Function &&) = default;
    Function &operator=(Function &&) = default;

    Function(const Function &other)
        : _M_base(other._M_base ? other._M_base->_M_clone() : nullptr) {}

    Function &operator=(const Function &other) {
        if (other._M_base) {
            _M_base = other._M_base->_M_clone();
        } else {
            _M_base = nullptr;
        }
    }

    explicit operator bool() const noexcept {
        return _M_base != nullptr;
    }

    bool operator==(std::nullptr_t) const noexcept {
        return _M_base == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept {
        return _M_base != nullptr;
    }

    // 函数调用运算符
    _Ret operator()(_Args... __args) const {
        if (!_M_base) [[unlikely]] {
            throw std::bad_function_call();
        }
        return _M_base->_M_call(std::forward<_Args>(__args)...);
    }

    const std::type_info &target_type() const noexcept {
        return _M_base ? _M_base->_M_type() : typeid(void);
    }

    template <class _Fn>
    _Fn *target() const noexcept {
        return _M_base && typeid(_Fn) == _M_base->_M_type()
                   ? std::addressof(
                         static_cast<_FuncImpl<_Fn> *>(_M_base.get())->_M_f)
                   : nullptr;
    }

    void swap(Function &__that) const noexcept {
        _M_base.swap(__that._M_base);
    }
};

} // namespace Marcus

#include <algorithm>
#include <containers/vector.hpp>
#include <functional>
#include <initializer_list>
#include <utility>

namespace Marcus {

template <typename _Tp, typename _Container = vector<_Tp>,
          typename _Compare = std::less<typename _Container::value_type>>
class priority_queue {
public:
    using container_type = _Container;
    using value_compare = _Compare;
    using value_type = typename _Container::value_type;
    using size_type = typename _Container::size_type;
    using reference = typename _Container::reference;
    using const_reference = typename _Container::const_reference;

protected:
    _Container _c;
    [[no_unique_address]] _Compare _comp;

public:
    priority_queue() = default;
    priority_queue(const priority_queue &) = default;
    priority_queue(priority_queue &&) = default;
    priority_queue &operator=(const priority_queue &) = default;
    priority_queue &operator=(priority_queue &&) = default;

    explicit priority_queue(const _Compare &__compare)
        : _c(),
          _comp(__compare) {}

    priority_queue(const _Compare &__compare, const _Container &__cont)
        : _c(__cont),
          _comp(__compare) {
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    priority_queue(const _Compare &__compare, _Container &&__cont)
        : _c(std::move(__cont)),
          _comp(__compare) {
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    template <typename _InputIt>
    priority_queue(_InputIt __first, _InputIt __last,
                   const _Compare &__compare = _Compare())
        : _c(__first, __last),
          _comp(__compare) {
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    template <class _InputIt>
    priority_queue(_InputIt __first, _InputIt __last, const _Compare &__compare,
                   const _Container &__cont)
        : _c(__cont),
          _comp(__compare) {
        _c.insert(_c.end(), __first, __last);
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    template <class _InputIt>
    priority_queue(_InputIt __first, _InputIt __last, const _Compare &__compare,
                   _Container &&__cont)
        : _c(std::move(__cont)),
          _comp(__compare) {
        _c.insert(_c.end(), __first, __last);
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    priority_queue(std::initializer_list<value_type> __ilist,
                   const _Compare &__compare = _Compare())
        : priority_queue(__ilist.begin(), __ilist.end(), __compare) {}

    priority_queue(std::initializer_list<value_type> __ilist,
                   const _Compare &__compare, const _Container &__cont)
        : _c(__cont),
          _comp(__compare) {
        _c.insert(_c.end(), __ilist.begin(), __ilist.end());
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    priority_queue(std::initializer_list<value_type> __ilist,
                   const _Compare &__compare, _Container &&__cont)
        : _c(std::move(__cont)),
          _comp(__compare) {
        _c.insert(_c.end(), __ilist.begin(), __ilist.end());
        std::make_heap(_c.begin(), _c.end(), _comp);
    }

    explicit priority_queue(const _Container &__cont)
        : priority_queue(_Compare(), __cont) {}

    explicit priority_queue(_Container &&__cont)
        : priority_queue(_Compare(), std::move(__cont)) {}

    const_reference top() const {
        return _c.front();
    }

    [[nodiscard]] bool empty() const noexcept {
        return _c.empty();
    }

    size_type size() const noexcept {
        return _c.size();
    }

    void push(const value_type &__val) {
        _c.push_back(__val);
        std::push_heap(_c.begin(), _c.end(), _comp);
    }

    void push(value_type &&__val) {
        _c.push_back(std::move(__val));
        std::push_heap(_c.begin(), _c.end(), _comp);
    }

    template <typename... _Args>
    void emplace(_Args... __args) {
        _c.emplace_back(std::forward<_Args>(__args)...);
        std::push_heap(_c.begin(), _c.end(), _comp);
    }

    void pop() {
        std::pop_heap(_c.begin(), _c.end(), _comp);
        _c.pop_back();
    }

    void swap(priority_queue &__other) noexcept(
        std::is_nothrow_swappable_v<_Container> &&
        std::is_nothrow_swappable_v<_Compare>) {
        using std::swap;
        swap(_c, __other._c);
        swap(_comp, __other._comp);
    }

    friend void swap(priority_queue &__lhs, priority_queue &__rhs) {
        __lhs.swap(__rhs);
    }
};

} // namespace Marcus

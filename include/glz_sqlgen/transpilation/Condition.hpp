#pragma once

#include "Operator.hpp"

namespace glz_sqlgen::transpilation {

/// Represents a SQL condition (WHERE clause component)
template <class LeftOperand, Operator Op, class RightOperand>
struct Condition {
    LeftOperand left;
    RightOperand right;

    static constexpr Operator op = Op;

    constexpr Condition(const LeftOperand& l, const RightOperand& r)
        : left(l), right(r) {}
};

/// Helper to create conditions
template <Operator Op, class L, class R>
constexpr auto make_condition(L&& left, R&& right) {
    return Condition<std::remove_cvref_t<L>, Op, std::remove_cvref_t<R>>{
        std::forward<L>(left), std::forward<R>(right)};
}

/// Wrapper to mark a condition
template <class T>
struct ConditionWrapper {
    T condition;

    constexpr ConditionWrapper(T&& cond) : condition(std::move(cond)) {}
    constexpr ConditionWrapper(const T& cond) : condition(cond) {}
};

template <class T>
constexpr auto make_condition_wrapper(T&& cond) {
    return ConditionWrapper<std::remove_cvref_t<T>>{std::forward<T>(cond)};
}

} // namespace glz_sqlgen::transpilation

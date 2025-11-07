#pragma once

#include "Operator.hpp"
#include "Value.hpp"

namespace glz_sqlgen::transpilation {

// Forward declaration
template <Operator Op, class Operand1, class Operand2>
struct Operation;

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

// Comparison operators for Operations to create Conditions
template <Operator Op, class O1, class O2, class T>
constexpr auto operator==(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator!=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::not_equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::less_than>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::less_equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::greater_than>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::greater_equal>(lhs, Value<T>{rhs}));
}

} // namespace glz_sqlgen::transpilation

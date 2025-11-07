#pragma once

#include "Operator.hpp"
#include "Value.hpp"

namespace glz_sqlgen::transpilation {

/// Represents a binary operation (e.g., col1 + col2, col * 5)
template <Operator Op, class Operand1, class Operand2>
struct Operation {
    Operand1 operand1;
    Operand2 operand2;

    static constexpr Operator op = Op;

    constexpr Operation(const Operand1& op1, const Operand2& op2)
        : operand1(op1), operand2(op2) {}
};

/// Helper to create operations
template <Operator Op, class T1, class T2>
constexpr auto make_operation(T1&& op1, T2&& op2) {
    return Operation<Op, std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>{
        std::forward<T1>(op1), std::forward<T2>(op2)};
}

// Operators for chaining operations
template <Operator Op1, class O1, class O2, class T>
constexpr auto operator+(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::plus>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator-(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::minus>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator*(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::multiplies>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator/(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::divides>(lhs, Value<T>{rhs});
}

} // namespace glz_sqlgen::transpilation

#pragma once

#include <string>
#include <tuple>
#include <string_view>

namespace glz_sqlgen::transpilation {

/// SQL function types
enum class FunctionType {
    // String functions
    concat,
    length,
    lower,
    upper,
    trim,
    ltrim,
    rtrim,
    replace,
    substring,

    // Math functions
    abs,
    ceil,
    floor,
    round,
    sqrt,
    exp,
    ln,
    log2,
    log10,
    pow,

    // Trigonometric functions
    sin,
    cos,
    tan,
    asin,
    acos,
    atan,

    // Date/time functions
    year,
    month,
    day,
    hour,
    minute,
    second,
    weekday,
    days_between,
    unixepoch,

    // Utility functions
    cast,
    coalesce
};

/// Convert function type to SQL function name
constexpr std::string_view function_type_to_sql(FunctionType type) {
    switch (type) {
        // String functions
        case FunctionType::concat: return "CONCAT";
        case FunctionType::length: return "LENGTH";
        case FunctionType::lower: return "LOWER";
        case FunctionType::upper: return "UPPER";
        case FunctionType::trim: return "TRIM";
        case FunctionType::ltrim: return "LTRIM";
        case FunctionType::rtrim: return "RTRIM";
        case FunctionType::replace: return "REPLACE";
        case FunctionType::substring: return "SUBSTR";

        // Math functions
        case FunctionType::abs: return "ABS";
        case FunctionType::ceil: return "CEIL";
        case FunctionType::floor: return "FLOOR";
        case FunctionType::round: return "ROUND";
        case FunctionType::sqrt: return "SQRT";
        case FunctionType::exp: return "EXP";
        case FunctionType::ln: return "LN";
        case FunctionType::log2: return "LOG2";
        case FunctionType::log10: return "LOG10";
        case FunctionType::pow: return "POW";

        // Trigonometric functions
        case FunctionType::sin: return "SIN";
        case FunctionType::cos: return "COS";
        case FunctionType::tan: return "TAN";
        case FunctionType::asin: return "ASIN";
        case FunctionType::acos: return "ACOS";
        case FunctionType::atan: return "ATAN";

        // Date/time functions - SQLite specific names
        case FunctionType::year: return "strftime";
        case FunctionType::month: return "strftime";
        case FunctionType::day: return "strftime";
        case FunctionType::hour: return "strftime";
        case FunctionType::minute: return "strftime";
        case FunctionType::second: return "strftime";
        case FunctionType::weekday: return "strftime";
        case FunctionType::days_between: return "julianday";
        case FunctionType::unixepoch: return "unixepoch";

        // Utility functions
        case FunctionType::cast: return "CAST";
        case FunctionType::coalesce: return "COALESCE";

        default: return "";
    }
}

/// SQL function with variadic arguments
template <FunctionType Type, class... ArgTypes>
struct Function {
    static constexpr FunctionType function_type = Type;
    std::tuple<ArgTypes...> arguments;

    // Constructor for multiple arguments
    constexpr Function(const ArgTypes&... args) : arguments(std::make_tuple(args...)) {}

    // Constructor for single argument (including tuple)
    template <class T>
    requires (sizeof...(ArgTypes) == 1)
    constexpr Function(T&& arg) : arguments(std::make_tuple(std::forward<T>(arg))) {}
};

/// Marker type for CAST target type
template <class T>
struct CastTarget {
    using type = T;
};

/// Specialized Function for CAST with target type
template <class TargetType, class ExprType>
struct CastFunction {
    static constexpr FunctionType function_type = FunctionType::cast;
    ExprType expression;

    constexpr CastFunction(const ExprType& expr) : expression(expr) {}
};

// Forward declarations for operations
enum class Operator;
template <Operator Op, class Operand1, class Operand2>
struct Operation;
template <class Left, Operator Op, class Right>
struct Condition;

// Arithmetic operators for Function types
template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator+(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::plus, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator-(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::minus, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator*(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::multiplies, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator/(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::divides, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator%(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::mod, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

// Comparison operators for Function types
template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator==(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator!=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator<(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator<=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator>(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator>=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

// Comparison operators for CastFunction types
template <class TargetType, class ExprType, class T>
constexpr auto operator==(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator!=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator<(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator<=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator>(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator>=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

} // namespace glz_sqlgen::transpilation

#pragma once

#include "transpilation_advanced.hpp"
#include <type_traits>

namespace sqlgen {

// ============================================================================
// STRING FUNCTIONS
// ============================================================================

/// CONCAT - Concatenate strings
template <class... ArgTypes>
auto concat(const ArgTypes&... args) {
    return transpilation::Function<transpilation::FunctionType::concat,
        typename transpilation::ToTranspilationType<std::remove_cvref_t<ArgTypes>>::Type...>{
        transpilation::to_transpilation_type(args)...
    };
}

/// LENGTH - String length
template <class ArgType>
constexpr auto length(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::length, std::remove_cvref_t<ArgType>>{arg};
}

/// LOWER - Convert to lowercase
template <class ArgType>
constexpr auto lower(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::lower, std::remove_cvref_t<ArgType>>{arg};
}

/// UPPER - Convert to uppercase
template <class ArgType>
constexpr auto upper(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::upper, std::remove_cvref_t<ArgType>>{arg};
}

/// TRIM - Trim whitespace from both ends
template <class ArgType>
constexpr auto trim(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::trim, std::remove_cvref_t<ArgType>>{arg};
}

/// LTRIM - Trim whitespace from left
template <class ArgType>
constexpr auto ltrim(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::ltrim, std::remove_cvref_t<ArgType>>{arg};
}

/// RTRIM - Trim whitespace from right
template <class ArgType>
constexpr auto rtrim(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::rtrim, std::remove_cvref_t<ArgType>>{arg};
}

/// REPLACE - Replace substring
template <class StrType, class FromType, class ToType>
auto replace(const StrType& str, const FromType& from, const ToType& to) {
    using Type1 = typename transpilation::ToTranspilationType<std::remove_cvref_t<StrType>>::Type;
    using Type2 = typename transpilation::ToTranspilationType<std::remove_cvref_t<FromType>>::Type;
    using Type3 = typename transpilation::ToTranspilationType<std::remove_cvref_t<ToType>>::Type;
    return transpilation::Function<transpilation::FunctionType::replace, Type1, Type2, Type3>{
        transpilation::to_transpilation_type(str),
        transpilation::to_transpilation_type(from),
        transpilation::to_transpilation_type(to)
    };
}

/// SUBSTRING - Extract substring (str, start, length)
template <class StrType, class StartType, class LengthType>
constexpr auto substring(const StrType& str, const StartType& start, const LengthType& length) {
    return transpilation::Function<transpilation::FunctionType::substring,
        std::remove_cvref_t<StrType>, std::remove_cvref_t<StartType>, std::remove_cvref_t<LengthType>>{str, start, length};
}

// ============================================================================
// MATH FUNCTIONS
// ============================================================================

/// ABS - Absolute value
template <class ArgType>
constexpr auto abs(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::abs, std::remove_cvref_t<ArgType>>{arg};
}

/// CEIL - Ceiling (round up)
template <class ArgType>
constexpr auto ceil(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::ceil, std::remove_cvref_t<ArgType>>{arg};
}

/// FLOOR - Floor (round down)
template <class ArgType>
constexpr auto floor(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::floor, std::remove_cvref_t<ArgType>>{arg};
}

/// ROUND - Round to n decimal places
template <class ArgType, class PrecisionType>
constexpr auto round(const ArgType& arg, const PrecisionType& precision) {
    return transpilation::Function<transpilation::FunctionType::round,
        std::remove_cvref_t<ArgType>, std::remove_cvref_t<PrecisionType>>{arg, precision};
}

/// ROUND - Round to nearest integer
template <class ArgType>
constexpr auto round(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::round, std::remove_cvref_t<ArgType>>{arg};
}

/// SQRT - Square root
template <class ArgType>
constexpr auto sqrt(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::sqrt, std::remove_cvref_t<ArgType>>{arg};
}

/// EXP - Exponential (e^x)
template <class ArgType>
constexpr auto exp(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::exp, std::remove_cvref_t<ArgType>>{arg};
}

/// LN - Natural logarithm
template <class ArgType>
constexpr auto ln(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::ln, std::remove_cvref_t<ArgType>>{arg};
}

/// LOG2 - Base-2 logarithm
template <class ArgType>
constexpr auto log2(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::log2, std::remove_cvref_t<ArgType>>{arg};
}

/// LOG10 - Base-10 logarithm
template <class ArgType>
constexpr auto log10(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::log10, std::remove_cvref_t<ArgType>>{arg};
}

/// POW - Power (base^exponent)
template <class BaseType, class ExpType>
constexpr auto pow(const BaseType& base, const ExpType& exponent) {
    return transpilation::Function<transpilation::FunctionType::pow,
        std::remove_cvref_t<BaseType>, std::remove_cvref_t<ExpType>>{base, exponent};
}

// ============================================================================
// TRIGONOMETRIC FUNCTIONS
// ============================================================================

/// SIN - Sine
template <class ArgType>
constexpr auto sin(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::sin, std::remove_cvref_t<ArgType>>{arg};
}

/// COS - Cosine
template <class ArgType>
constexpr auto cos(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::cos, std::remove_cvref_t<ArgType>>{arg};
}

/// TAN - Tangent
template <class ArgType>
constexpr auto tan(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::tan, std::remove_cvref_t<ArgType>>{arg};
}

/// ASIN - Arcsine
template <class ArgType>
constexpr auto asin(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::asin, std::remove_cvref_t<ArgType>>{arg};
}

/// ACOS - Arccosine
template <class ArgType>
constexpr auto acos(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::acos, std::remove_cvref_t<ArgType>>{arg};
}

/// ATAN - Arctangent
template <class ArgType>
constexpr auto atan(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::atan, std::remove_cvref_t<ArgType>>{arg};
}

// ============================================================================
// DATE/TIME FUNCTIONS
// ============================================================================

/// YEAR - Extract year from date
template <class ArgType>
constexpr auto year(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::year, std::remove_cvref_t<ArgType>>{arg};
}

/// MONTH - Extract month from date
template <class ArgType>
constexpr auto month(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::month, std::remove_cvref_t<ArgType>>{arg};
}

/// DAY - Extract day from date
template <class ArgType>
constexpr auto day(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::day, std::remove_cvref_t<ArgType>>{arg};
}

/// HOUR - Extract hour from datetime
template <class ArgType>
constexpr auto hour(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::hour, std::remove_cvref_t<ArgType>>{arg};
}

/// MINUTE - Extract minute from datetime
template <class ArgType>
constexpr auto minute(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::minute, std::remove_cvref_t<ArgType>>{arg};
}

/// SECOND - Extract second from datetime
template <class ArgType>
constexpr auto second(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::second, std::remove_cvref_t<ArgType>>{arg};
}

/// WEEKDAY - Day of week (0 = Sunday)
template <class ArgType>
constexpr auto weekday(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::weekday, std::remove_cvref_t<ArgType>>{arg};
}

/// DAYS_BETWEEN - Days between two dates
template <class Date1Type, class Date2Type>
auto days_between(const Date1Type& date1, const Date2Type& date2) {
    using Type1 = typename transpilation::ToTranspilationType<std::remove_cvref_t<Date1Type>>::Type;
    using Type2 = typename transpilation::ToTranspilationType<std::remove_cvref_t<Date2Type>>::Type;
    return transpilation::Function<transpilation::FunctionType::days_between, Type1, Type2>{
        transpilation::to_transpilation_type(date1),
        transpilation::to_transpilation_type(date2)
    };
}

/// UNIXEPOCH - Convert to Unix timestamp
template <class ArgType>
constexpr auto unixepoch(const ArgType& arg) {
    return transpilation::Function<transpilation::FunctionType::unixepoch, std::remove_cvref_t<ArgType>>{arg};
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/// CAST - Type casting
template <class TargetType, class ExprType>
constexpr auto cast(const ExprType& expr) {
    return transpilation::CastFunction<TargetType, std::remove_cvref_t<ExprType>>{expr};
}

/// COALESCE - Return first non-NULL value
template <class... ArgTypes>
auto coalesce(const ArgTypes&... args) {
    return transpilation::Function<transpilation::FunctionType::coalesce,
        typename transpilation::ToTranspilationType<std::remove_cvref_t<ArgTypes>>::Type...>{
        transpilation::to_transpilation_type(args)...
    };
}

} // namespace sqlgen

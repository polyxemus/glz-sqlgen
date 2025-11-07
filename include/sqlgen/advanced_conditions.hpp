#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include "transpilation_core.hpp"

namespace sqlgen {

// Forward declarations
template <glz::string_literal Name, glz::string_literal Alias>
struct Col;

namespace advanced {

/// LIKE pattern matching
template <class ColType, class PatternType>
auto like(const ColType& col, const PatternType& pattern) {
    return transpilation::make_condition_wrapper(
        transpilation::make_condition<transpilation::Operator::like>(col, transpilation::Value<PatternType>{pattern}));
}

/// NOT LIKE pattern matching
template <class ColType, class PatternType>
auto not_like(const ColType& col, const PatternType& pattern) {
    return transpilation::make_condition_wrapper(
        transpilation::make_condition<transpilation::Operator::not_like>(col, transpilation::Value<PatternType>{pattern}));
}

/// ILIKE case-insensitive pattern matching (PostgreSQL)
template <class ColType, class PatternType>
auto ilike(const ColType& col, const PatternType& pattern) {
    return transpilation::make_condition_wrapper(
        transpilation::make_condition<transpilation::Operator::ilike>(col, transpilation::Value<PatternType>{pattern}));
}

/// NOT ILIKE case-insensitive pattern matching
template <class ColType, class PatternType>
auto not_ilike(const ColType& col, const PatternType& pattern) {
    return transpilation::make_condition_wrapper(
        transpilation::make_condition<transpilation::Operator::not_ilike>(col, transpilation::Value<PatternType>{pattern}));
}

/// IS NULL check
template <class ColType>
struct IsNullCondition {
    ColType column;

    constexpr IsNullCondition(const ColType& col) : column(col) {}
};

/// IS NOT NULL check
template <class ColType>
struct IsNotNullCondition {
    ColType column;

    constexpr IsNotNullCondition(const ColType& col) : column(col) {}
};

/// IN operator with variadic values
template <class ColType, class... ValueTypes>
struct InCondition {
    ColType column;
    std::tuple<ValueTypes...> values;

    constexpr InCondition(const ColType& col, ValueTypes... vals)
        : column(col), values(std::make_tuple(vals...)) {}
};

/// NOT IN operator with variadic values
template <class ColType, class... ValueTypes>
struct NotInCondition {
    ColType column;
    std::tuple<ValueTypes...> values;

    constexpr NotInCondition(const ColType& col, ValueTypes... vals)
        : column(col), values(std::make_tuple(vals...)) {}
};

/// BETWEEN operator
template <class ColType, class LowerType, class UpperType>
struct BetweenCondition {
    ColType column;
    LowerType lower;
    UpperType upper;

    constexpr BetweenCondition(const ColType& col, const LowerType& low, const UpperType& up)
        : column(col), lower(low), upper(up) {}
};

/// NOT BETWEEN operator
template <class ColType, class LowerType, class UpperType>
struct NotBetweenCondition {
    ColType column;
    LowerType lower;
    UpperType upper;

    constexpr NotBetweenCondition(const ColType& col, const LowerType& low, const UpperType& up)
        : column(col), lower(low), upper(up) {}
};

/// IS NULL
template <class ColType>
constexpr auto is_null(const ColType& col) {
    return transpilation::ConditionWrapper{IsNullCondition{col}};
}

/// IS NOT NULL
template <class ColType>
constexpr auto is_not_null(const ColType& col) {
    return transpilation::ConditionWrapper{IsNotNullCondition{col}};
}

/// IN with variadic values
template <class ColType, class... ValueTypes>
constexpr auto in(const ColType& col, const ValueTypes&... values) {
    return transpilation::ConditionWrapper{InCondition{col, values...}};
}

/// NOT IN with variadic values
template <class ColType, class... ValueTypes>
constexpr auto not_in(const ColType& col, const ValueTypes&... values) {
    return transpilation::ConditionWrapper{NotInCondition{col, values...}};
}

/// BETWEEN
template <class ColType, class LowerType, class UpperType>
constexpr auto between(const ColType& col, const LowerType& lower, const UpperType& upper) {
    return transpilation::ConditionWrapper{BetweenCondition{col, lower, upper}};
}

/// NOT BETWEEN
template <class ColType, class LowerType, class UpperType>
constexpr auto not_between(const ColType& col, const LowerType& lower, const UpperType& upper) {
    return transpilation::ConditionWrapper{NotBetweenCondition{col, lower, upper}};
}

} // namespace advanced

// Export to main namespace
using advanced::like;
using advanced::not_like;
using advanced::ilike;
using advanced::not_ilike;
using advanced::is_null;
using advanced::is_not_null;
using advanced::in;
using advanced::not_in;
using advanced::between;
using advanced::not_between;

} // namespace sqlgen

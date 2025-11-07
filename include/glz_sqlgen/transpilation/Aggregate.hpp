#pragma once

#include <string_view>
#include <variant>
#include "Desc.hpp"
#include "Value.hpp"

namespace glz_sqlgen::transpilation {

/// Aggregate function types
enum class AggregateType {
    count,
    count_distinct,
    sum,
    avg,
    min,
    max
};

/// Convert AggregateType to SQL function name
constexpr std::string_view aggregate_type_to_sql(AggregateType type) {
    switch (type) {
        case AggregateType::count: return "COUNT";
        case AggregateType::count_distinct: return "COUNT";
        case AggregateType::sum: return "SUM";
        case AggregateType::avg: return "AVG";
        case AggregateType::min: return "MIN";
        case AggregateType::max: return "MAX";
    }
    return "";
}

/// Marker type for COUNT(*)
struct CountStar {};

/// Aggregate function representation
/// ExprType: The expression being aggregated (column, operation, or CountStar)
template <AggregateType Type, class ExprType>
struct Aggregate {
    static constexpr AggregateType aggregate_type = Type;

    ExprType expression;

    constexpr Aggregate() = default;
    constexpr explicit Aggregate(ExprType expr) : expression(expr) {}

    constexpr bool is_count_star() const noexcept {
        return std::is_same_v<ExprType, CountStar>;
    }

    constexpr bool is_distinct() const noexcept {
        return Type == AggregateType::count_distinct;
    }

    // Return descending order marker for ORDER BY
    constexpr auto desc() const noexcept {
        return make_desc(*this);
    }
};

// Forward declaration for Condition
template <class Left, Operator Op, class Right>
struct Condition;

// Comparison operators for Aggregate types
template <AggregateType Type, class ExprType, class T>
constexpr auto operator==(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator!=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator<(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator<=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator>(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator>=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

} // namespace glz_sqlgen::transpilation

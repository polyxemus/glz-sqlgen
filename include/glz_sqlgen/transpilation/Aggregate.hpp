#pragma once

#include <string_view>
#include <variant>

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
};

} // namespace glz_sqlgen::transpilation

#pragma once

#include "transpilation/Aggregate.hpp"
#include "transpilation/Col.hpp"

namespace glz_sqlgen {

/// COUNT(*) - Count all rows
inline constexpr auto count_star() {
    return transpilation::Aggregate<transpilation::AggregateType::count, transpilation::CountStar>{};
}

/// COUNT(column) - Count non-NULL values in column
template <class ExprType>
constexpr auto count(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::count, std::remove_cvref_t<ExprType>>{expr};
}

/// COUNT(DISTINCT column) - Count distinct non-NULL values
template <class ExprType>
constexpr auto count_distinct(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::count_distinct, std::remove_cvref_t<ExprType>>{expr};
}

/// SUM(column) - Sum of all non-NULL values
template <class ExprType>
constexpr auto sum(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::sum, std::remove_cvref_t<ExprType>>{expr};
}

/// AVG(column) - Average of all non-NULL values
template <class ExprType>
constexpr auto avg(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::avg, std::remove_cvref_t<ExprType>>{expr};
}

/// MIN(column) - Minimum value
template <class ExprType>
constexpr auto min(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::min, std::remove_cvref_t<ExprType>>{expr};
}

/// MAX(column) - Maximum value
template <class ExprType>
constexpr auto max(const ExprType& expr) {
    return transpilation::Aggregate<transpilation::AggregateType::max, std::remove_cvref_t<ExprType>>{expr};
}

} // namespace glz_sqlgen

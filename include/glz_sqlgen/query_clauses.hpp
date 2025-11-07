#pragma once

#include <type_traits>
#include <tuple>
#include <cstddef>
#include <optional>
#include <glaze/util/string_literal.hpp>
#include "core.hpp"
#include "transpilation/Join.hpp"
#include "transpilation/Aggregate.hpp"
#include "transpilation/Col.hpp"

namespace glz_sqlgen {

// ============================================================================
// WHERE Clause
// ============================================================================

/// Wrapper for WHERE clause conditions
template <class ConditionType>
struct Where {
    ConditionType condition;
};

/// Create a WHERE clause from a condition
template <class ConditionType>
inline auto where(const ConditionType& _cond) {
    return Where<std::remove_cvref_t<ConditionType>>{.condition = _cond};
}

// ============================================================================
// JOIN Clauses
// ============================================================================

/// INNER JOIN wrapper
template <class TableType, glz::string_literal Alias = "", class ConditionType = Nothing>
struct InnerJoin {
    using JoinType = transpilation::Join<transpilation::JoinType::inner, TableType, Alias, ConditionType>;
    JoinType join_clause;

    constexpr InnerJoin() = default;
    constexpr explicit InnerJoin(ConditionType cond) : join_clause(cond) {}
};

/// LEFT OUTER JOIN wrapper
template <class TableType, glz::string_literal Alias = "", class ConditionType = Nothing>
struct LeftJoin {
    using JoinType = transpilation::Join<transpilation::JoinType::left, TableType, Alias, ConditionType>;
    JoinType join_clause;

    constexpr LeftJoin() = default;
    constexpr explicit LeftJoin(ConditionType cond) : join_clause(cond) {}
};

/// RIGHT OUTER JOIN wrapper
template <class TableType, glz::string_literal Alias = "", class ConditionType = Nothing>
struct RightJoin {
    using JoinType = transpilation::Join<transpilation::JoinType::right, TableType, Alias, ConditionType>;
    JoinType join_clause;

    constexpr RightJoin() = default;
    constexpr explicit RightJoin(ConditionType cond) : join_clause(cond) {}
};

/// FULL OUTER JOIN wrapper
template <class TableType, glz::string_literal Alias = "", class ConditionType = Nothing>
struct FullJoin {
    using JoinType = transpilation::Join<transpilation::JoinType::full, TableType, Alias, ConditionType>;
    JoinType join_clause;

    constexpr FullJoin() = default;
    constexpr explicit FullJoin(ConditionType cond) : join_clause(cond) {}
};

/// CROSS JOIN wrapper (no condition required)
template <class TableType, glz::string_literal Alias = "">
struct CrossJoin {
    using JoinType = transpilation::Join<transpilation::JoinType::cross, TableType, Alias, Nothing>;
    JoinType join_clause;

    constexpr CrossJoin() = default;
};

/// Create an INNER JOIN with ON condition
template <class TableType, class ConditionType>
constexpr auto inner_join(const ConditionType& condition) {
    return InnerJoin<TableType, "", std::remove_cvref_t<ConditionType>>{condition};
}

/// Create an INNER JOIN with table alias and ON condition
template <class TableType, glz::string_literal Alias, class ConditionType>
constexpr auto inner_join(const ConditionType& condition) {
    return InnerJoin<TableType, Alias, std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a LEFT OUTER JOIN with ON condition
template <class TableType, class ConditionType>
constexpr auto left_join(const ConditionType& condition) {
    return LeftJoin<TableType, "", std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a LEFT OUTER JOIN with table alias and ON condition
template <class TableType, glz::string_literal Alias, class ConditionType>
constexpr auto left_join(const ConditionType& condition) {
    return LeftJoin<TableType, Alias, std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a RIGHT OUTER JOIN with ON condition
template <class TableType, class ConditionType>
constexpr auto right_join(const ConditionType& condition) {
    return RightJoin<TableType, "", std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a RIGHT OUTER JOIN with table alias and ON condition
template <class TableType, glz::string_literal Alias, class ConditionType>
constexpr auto right_join(const ConditionType& condition) {
    return RightJoin<TableType, Alias, std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a FULL OUTER JOIN with ON condition
template <class TableType, class ConditionType>
constexpr auto full_join(const ConditionType& condition) {
    return FullJoin<TableType, "", std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a FULL OUTER JOIN with table alias and ON condition
template <class TableType, glz::string_literal Alias, class ConditionType>
constexpr auto full_join(const ConditionType& condition) {
    return FullJoin<TableType, Alias, std::remove_cvref_t<ConditionType>>{condition};
}

/// Create a CROSS JOIN (no condition)
template <class TableType>
constexpr auto cross_join() {
    return CrossJoin<TableType>{};
}

/// Create a CROSS JOIN with table alias
template <class TableType, glz::string_literal Alias>
constexpr auto cross_join() {
    return CrossJoin<TableType, Alias>{};
}

// ============================================================================
// ORDER BY Clause
// ============================================================================

/// Wrapper for ORDER BY clause
template <class... ColTypes>
struct OrderBy {
    std::tuple<ColTypes...> columns;
};

/// Create an ORDER BY clause from columns
template <class... ColTypes>
auto order_by(const ColTypes&... cols) {
    return OrderBy<ColTypes...>{.columns = std::make_tuple(cols...)};
}

// ============================================================================
// LIMIT Clause
// ============================================================================

/// Represents LIMIT and optional OFFSET
struct Limit {
    size_t limit_value;
    std::optional<size_t> offset_value;
};

/// Create a LIMIT clause
inline auto limit(size_t val) {
    return Limit{.limit_value = val, .offset_value = std::nullopt};
}

/// Create a LIMIT with OFFSET clause
inline auto limit(size_t limit_val, size_t offset_val) {
    return Limit{.limit_value = limit_val, .offset_value = offset_val};
}

// ============================================================================
// GROUP BY Clause
// ============================================================================

/// GROUP BY clause wrapper
template <class... ColTypes>
struct GroupBy {
    std::tuple<ColTypes...> columns;

    constexpr GroupBy(ColTypes... cols) : columns(std::make_tuple(cols...)) {}
};

/// Create a GROUP BY clause
template <class... ColTypes>
constexpr auto group_by(const ColTypes&... columns) {
    return GroupBy<std::remove_cvref_t<ColTypes>...>{columns...};
}

// ============================================================================
// HAVING Clause
// ============================================================================

/// Wrapper for HAVING clause conditions
template <class ConditionType>
struct Having {
    ConditionType condition;
};

/// Create a HAVING clause from a condition
template <class ConditionType>
inline auto having(const ConditionType& _cond) {
    return Having<std::remove_cvref_t<ConditionType>>{.condition = _cond};
}

// ============================================================================
// Aggregate Functions
// ============================================================================

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

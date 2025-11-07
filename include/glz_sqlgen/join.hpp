#pragma once

#include <glaze/util/string_literal.hpp>
#include "transpilation/Join.hpp"
#include "common_types.hpp"

namespace glz_sqlgen {

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

} // namespace glz_sqlgen

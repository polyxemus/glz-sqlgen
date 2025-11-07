#pragma once

#include <string_view>
#include <glaze/util/string_literal.hpp>

namespace glz_sqlgen::transpilation {

/// JOIN types
enum class JoinType {
    inner,
    left,
    right,
    full,
    cross
};

/// Convert JoinType to SQL keyword
constexpr std::string_view join_type_to_sql(JoinType type) {
    switch (type) {
        case JoinType::inner: return "INNER JOIN";
        case JoinType::left: return "LEFT OUTER JOIN";
        case JoinType::right: return "RIGHT OUTER JOIN";
        case JoinType::full: return "FULL OUTER JOIN";
        case JoinType::cross: return "CROSS JOIN";
    }
    return "";
}

/// Single JOIN clause representation
/// TableType: The table being joined
/// Alias: Optional table alias
/// ConditionType: The ON condition (Nothing for CROSS JOIN)
template <JoinType Type, class TableType, glz::string_literal Alias = "", class ConditionType = void>
struct Join {
    static constexpr JoinType join_type = Type;
    static constexpr std::string_view alias = Alias.sv();

    ConditionType condition;

    constexpr Join() = default;
    constexpr explicit Join(ConditionType cond) : condition(cond) {}

    constexpr bool has_alias() const noexcept {
        return !alias.empty();
    }

    constexpr std::string_view get_alias() const noexcept {
        return alias;
    }
};

/// List of JOINs (supports multiple JOINs in a single query)
template <class... Joins>
struct JoinList {
    std::tuple<Joins...> joins;

    constexpr JoinList() = default;
    constexpr explicit JoinList(Joins... js) : joins(std::make_tuple(js...)) {}
};

} // namespace glz_sqlgen::transpilation

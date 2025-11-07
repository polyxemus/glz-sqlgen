#pragma once

#include <string>
#include <tuple>
#include "Join.hpp"
#include "table_info.hpp"
#include "quote.hpp"
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate SQL for a single JOIN clause
template <JoinType Type, class TableType, glz::string_literal Alias, class ConditionType>
std::string join_sql(const Join<Type, TableType, Alias, ConditionType>& join) {
    std::string sql;

    // Add JOIN type
    sql += join_type_to_sql(Type);
    sql += " ";

    // Add table name
    sql += quote_identifier(get_table_name<TableType>());

    // Add alias if present
    if (join.has_alias()) {
        sql += " AS ";
        sql += quote_identifier(join.get_alias());
    }

    // Add ON condition (except for CROSS JOIN)
    if constexpr (Type != JoinType::cross) {
        sql += " ON ";
        sql += to_sql(join.condition);
    }

    return sql;
}

/// Generate SQL for a list of JOINs
template <class... Joins>
std::string joins_sql(const JoinList<Joins...>& join_list) {
    std::string sql;
    bool first = true;

    std::apply([&](const auto&... joins) {
        (([&] {
            if (!first) {
                sql += " ";
            }
            sql += join_sql(joins);
            first = false;
        }()), ...);
    }, join_list.joins);

    return sql;
}

} // namespace glz_sqlgen::transpilation

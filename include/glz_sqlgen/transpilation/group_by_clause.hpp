#pragma once

#include <string>
#include <tuple>
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate SQL for GROUP BY clause
template <class... ColTypes>
std::string group_by_sql(const std::tuple<ColTypes...>& columns) {
    std::string sql = "GROUP BY ";

    bool first = true;
    std::apply([&](const auto&... cols) {
        (([&] {
            if (!first) {
                sql += ", ";
            }
            sql += to_sql(cols);
            first = false;
        }()), ...);
    }, columns);

    return sql;
}

} // namespace glz_sqlgen::transpilation

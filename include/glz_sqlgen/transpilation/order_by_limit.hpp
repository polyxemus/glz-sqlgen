#pragma once

#include <string>
#include <tuple>
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate ORDER BY SQL from tuple of columns
template <class... ColTypes>
std::string order_by_sql(const std::tuple<ColTypes...>& columns) {
    std::string result = "ORDER BY ";
    bool first = true;

    std::apply([&](const auto&... cols) {
        (([&] {
            if (first) {
                result += to_sql(cols);
                first = false;
            } else {
                result += ", " + to_sql(cols);
            }
        }()), ...);
    }, columns);

    return result;
}

/// Generate LIMIT SQL
inline std::string limit_sql(size_t limit_value, const std::optional<size_t>& offset_value) {
    std::string result = "LIMIT " + std::to_string(limit_value);
    if (offset_value.has_value()) {
        result += " OFFSET " + std::to_string(offset_value.value());
    }
    return result;
}

} // namespace glz_sqlgen::transpilation

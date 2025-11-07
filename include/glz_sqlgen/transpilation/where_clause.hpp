#pragma once

#include <string>
#include "Condition.hpp"
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate a WHERE clause from a condition
template <class CondType>
std::string where_clause(const CondType& condition) {
    return "WHERE " + to_sql(condition);
}

/// Generate a WHERE clause from a condition wrapper
template <class T>
std::string where_clause(const ConditionWrapper<T>& wrapper) {
    return "WHERE " + to_sql(wrapper.condition);
}

/// Combine multiple conditions with AND
template <class... Conditions>
std::string where_clause_and(const Conditions&... conditions) {
    std::string result = "WHERE ";
    size_t idx = 0;

    ([&] {
        if (idx > 0) result += " AND ";
        result += to_sql(conditions);
        ++idx;
    }(), ...);

    return result;
}

/// Combine multiple conditions with OR
template <class... Conditions>
std::string where_clause_or(const Conditions&... conditions) {
    std::string result = "WHERE ";
    size_t idx = 0;

    ([&] {
        if (idx > 0) result += " OR ";
        result += to_sql(conditions);
        ++idx;
    }(), ...);

    return result;
}

} // namespace glz_sqlgen::transpilation

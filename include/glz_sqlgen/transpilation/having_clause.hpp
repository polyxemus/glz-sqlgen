#pragma once

#include <string>
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate SQL for HAVING clause
template <class ConditionType>
std::string having_clause(const ConditionType& condition) {
    return "HAVING " + to_sql(condition);
}

} // namespace glz_sqlgen::transpilation

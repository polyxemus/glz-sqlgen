#pragma once

#include <string>
#include "Aggregate.hpp"
#include "to_sql_string.hpp"

namespace glz_sqlgen::transpilation {

/// Generate SQL for aggregate function
template <AggregateType Type, class ExprType>
std::string aggregate_sql(const Aggregate<Type, ExprType>& agg) {
    std::string sql;

    // Add function name
    sql += aggregate_type_to_sql(Type);
    sql += "(";

    // Handle different cases
    if constexpr (std::is_same_v<ExprType, CountStar>) {
        // COUNT(*)
        sql += "*";
    } else {
        // Add DISTINCT for COUNT(DISTINCT col)
        if (agg.is_distinct()) {
            sql += "DISTINCT ";
        }
        // Add expression
        sql += to_sql(agg.expression);
    }

    sql += ")";
    return sql;
}

} // namespace glz_sqlgen::transpilation

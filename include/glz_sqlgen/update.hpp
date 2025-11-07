#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include "common_types.hpp"
#include "where.hpp"
#include "transpilation/table_info.hpp"
#include "transpilation/where_clause.hpp"
#include "transpilation/to_sql_string.hpp"
#include "transpilation/Set.hpp"
#include "transpilation/quote.hpp"

namespace glz_sqlgen {

/// UPDATE query builder
template <class TableType, class SetsTuple, class WhereType = Nothing>
struct Update {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "UPDATE ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());
        sql += " SET ";

        // Add SET clauses
        bool first = true;
        std::apply([&](const auto&... sets) {
            (([&] {
                if (first) {
                    sql += transpilation::to_sql(sets);
                    first = false;
                } else {
                    sql += ", " + transpilation::to_sql(sets);
                }
            }()), ...);
        }, sets_);

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        return sql;
    }

    /// Pipe operator for WHERE clause
    template <class ConditionType>
    friend auto operator|(const Update& u, const Where<ConditionType>& w) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call where() twice");

        return Update<TableType, SetsTuple, ConditionType>{
            .sets_ = u.sets_,
            .where_ = w.condition
        };
    }

    SetsTuple sets_;
    WhereType where_;
};

/// Create an UPDATE Table SET ... query
template <class TableType, class... SetTypes>
auto update(const SetTypes&... sets) {
    static_assert(sizeof...(sets) > 0, "Must update at least one column");
    using SetsTuple = std::tuple<SetTypes...>;
    return Update<TableType, SetsTuple>{
        .sets_ = std::make_tuple(sets...)
    };
}

/// Helper function to create SET clauses
template <class Col, class Value>
auto set(const Col& col, const Value& val) {
    return transpilation::make_set(col, transpilation::Value<Value>{val});
}

} // namespace glz_sqlgen

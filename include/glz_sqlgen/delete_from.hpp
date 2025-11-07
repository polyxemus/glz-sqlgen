#pragma once

#include <string>
#include <type_traits>
#include "common_types.hpp"
#include "where.hpp"
#include "transpilation/table_info.hpp"
#include "transpilation/where_clause.hpp"
#include "transpilation/quote.hpp"

namespace glz_sqlgen {

/// DELETE FROM query builder
template <class TableType, class WhereType = Nothing>
struct DeleteFrom {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "DELETE FROM ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        return sql;
    }

    /// Pipe operator for WHERE clause
    template <class ConditionType>
    friend auto operator|(const DeleteFrom& d, const Where<ConditionType>& w) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call where() twice");

        return DeleteFrom<TableType, ConditionType>{
            .where_ = w.condition
        };
    }

    WhereType where_;
};

/// Create a DELETE FROM Table query
template <class TableType>
auto delete_from() {
    return DeleteFrom<TableType>{};
}

} // namespace glz_sqlgen

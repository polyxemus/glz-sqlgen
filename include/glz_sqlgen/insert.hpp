#pragma once

#include <string>
#include <vector>
#include <ranges>
#include "transpilation/table_info.hpp"
#include "transpilation/field_list.hpp"
#include "transpilation/quote.hpp"

namespace glz_sqlgen {

/// INSERT query builder
template <class TableType>
struct Insert {
    /// Convert to SQL string (returns statement with placeholders)
    std::string to_sql() const {
        std::string sql = or_replace_ ? "INSERT OR REPLACE INTO " : "INSERT INTO ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());
        sql += " (";
        sql += transpilation::insert_field_list<TableType>();
        sql += ") VALUES (";
        sql += transpilation::insert_placeholders<TableType>();
        sql += ")";

        return sql;
    }

    bool or_replace_ = false;
};

/// Create an INSERT INTO Table query
template <class TableType>
auto insert() {
    return Insert<TableType>{.or_replace_ = false};
}

/// Create an INSERT OR REPLACE INTO Table query
template <class TableType>
auto insert_or_replace() {
    return Insert<TableType>{.or_replace_ = true};
}

} // namespace glz_sqlgen

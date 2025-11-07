#pragma once

#include <string>
#include "transpilation/table_info.hpp"

namespace glz_sqlgen {

/// CREATE TABLE query builder
template <class TableType>
struct CreateTable {
    /// Convert to SQL string
    std::string to_sql() const {
        return transpilation::create_table_sql<TableType>(if_not_exists_);
    }

    bool if_not_exists_ = false;
};

/// Create a CREATE TABLE query
template <class TableType>
auto create_table(bool if_not_exists = false) {
    return CreateTable<TableType>{.if_not_exists_ = if_not_exists};
}

} // namespace glz_sqlgen

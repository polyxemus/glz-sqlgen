#pragma once

#include <sqlite3.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../Result.hpp"

namespace glz_sqlgen::sqlite {

/// Iterator over SQL query results
/// Returns rows as vectors of optional strings (NULL -> std::nullopt)
class Iterator {
public:
    using Row = std::vector<std::optional<std::string>>;

    /// Construct from prepared statement
    /// Takes ownership of stmt via shared_ptr with custom deleter
    Iterator(sqlite3_stmt* stmt, sqlite3* conn);

    /// Check if we've reached the end of results
    bool end() const { return end_; }

    /// Get the next row
    /// Returns std::nullopt if at end
    std::optional<Row> next();

    /// Get number of columns
    int column_count() const { return num_cols_; }

private:
    void step();

    bool end_;
    int num_cols_;
    std::shared_ptr<sqlite3_stmt> stmt_;
    std::shared_ptr<sqlite3> conn_;  // Keep connection alive
};

} // namespace glz_sqlgen::sqlite

#pragma once

#include <string>
#include <vector>
#include <concepts>

namespace glz_sqlgen {

/// A simple query builder class for demonstration purposes
class QueryBuilder {
public:
    QueryBuilder() = default;

    /// Add a SELECT clause
    QueryBuilder& select(const std::vector<std::string>& columns);

    /// Add a FROM clause
    QueryBuilder& from(const std::string& table);

    /// Add a WHERE clause
    QueryBuilder& where(const std::string& condition);

    /// Build and return the SQL query string
    [[nodiscard]] std::string build() const;

    /// Reset the builder to empty state
    void reset();

private:
    std::vector<std::string> columns_;
    std::string table_;
    std::string where_clause_;
};

} // namespace glz_sqlgen

#pragma once

#include <sqlite3.h>
#include <memory>
#include <string>
#include "../core.hpp"
#include "Iterator.hpp"

namespace sqlgen::sqlite {

/// SQLite database connection
class Connection {
public:
    /// Create connection to database file
    /// Use ":memory:" for in-memory database
    static Result<Connection> connect(const std::string& filename = ":memory:");

    /// Destructor closes the connection
    ~Connection() = default;

    /// Move constructor and assignment
    Connection(Connection&&) = default;
    Connection& operator=(Connection&&) = default;

    /// No copying
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    /// Execute SQL statement (INSERT, UPDATE, DELETE, CREATE, etc.)
    /// Does not return results
    Result<Nothing> execute(const std::string& sql);

    /// Execute query and return iterator over results
    Result<Iterator> query(const std::string& sql);

    /// Begin a transaction
    Result<Nothing> begin_transaction();

    /// Commit the current transaction
    Result<Nothing> commit();

    /// Rollback the current transaction
    Result<Nothing> rollback();

    /// Execute a query builder and return SQL
    template <class QueryBuilder>
    std::string to_sql(const QueryBuilder& builder) {
        return builder.to_sql();
    }

    /// Execute a query builder
    template <class QueryBuilder>
    Result<Nothing> execute(const QueryBuilder& builder) {
        return execute(builder.to_sql());
    }

private:
    /// Private constructor - use connect() factory
    explicit Connection(std::shared_ptr<sqlite3> conn) : conn_(std::move(conn)) {}

    std::shared_ptr<sqlite3> conn_;
};

} // namespace sqlgen::sqlite

#include "sqlgen/sqlite/Connection.hpp"
#include <sstream>

namespace sqlgen::sqlite {

Result<Connection> Connection::connect(const std::string& filename) {
    sqlite3* raw_conn = nullptr;
    int rc = sqlite3_open(filename.c_str(), &raw_conn);

    if (rc != SQLITE_OK) {
        std::string err_msg = raw_conn ? sqlite3_errmsg(raw_conn) : "Unknown error";
        if (raw_conn) {
            sqlite3_close(raw_conn);
        }
        return error("Failed to open database: " + err_msg);
    }

    // Wrap in shared_ptr with custom deleter
    auto conn = std::shared_ptr<sqlite3>(raw_conn, [](sqlite3* db) {
        if (db) sqlite3_close(db);
    });

    return Connection(std::move(conn));
}

Result<Nothing> Connection::execute(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(conn_.get(), sql.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        std::string error_str = err_msg ? err_msg : "Unknown error";
        if (err_msg) {
            sqlite3_free(err_msg);
        }
        return error("Failed to execute SQL: " + error_str);
    }

    return Nothing{};
}

Result<Iterator> Connection::query(const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(conn_.get(), sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::string err_msg = sqlite3_errmsg(conn_.get());
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        return error("Failed to prepare statement: " + err_msg);
    }

    return Iterator(stmt, conn_.get());
}

Result<Nothing> Connection::begin_transaction() {
    return execute(std::string("BEGIN TRANSACTION"));
}

Result<Nothing> Connection::commit() {
    return execute(std::string("COMMIT"));
}

Result<Nothing> Connection::rollback() {
    return execute(std::string("ROLLBACK"));
}

} // namespace sqlgen::sqlite

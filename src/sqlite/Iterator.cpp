#include "glz_sqlgen/sqlite/Iterator.hpp"
#include <cstring>

namespace glz_sqlgen::sqlite {

Iterator::Iterator(sqlite3_stmt* stmt, sqlite3* conn)
    : end_(false),
      num_cols_(sqlite3_column_count(stmt)),
      stmt_(stmt, [](sqlite3_stmt* s) { if (s) sqlite3_finalize(s); }),
      conn_(conn, [](sqlite3*) {}) // Don't close connection - it's owned elsewhere
{
    step();  // Step to first row
}

void Iterator::step() {
    if (end_) return;

    int rc = sqlite3_step(stmt_.get());
    if (rc == SQLITE_ROW) {
        end_ = false;
    } else if (rc == SQLITE_DONE) {
        end_ = true;
    } else {
        end_ = true;
    }
}

std::optional<Iterator::Row> Iterator::next() {
    if (end_) {
        return std::nullopt;
    }

    Row row;
    row.reserve(num_cols_);

    for (int i = 0; i < num_cols_; ++i) {
        if (sqlite3_column_type(stmt_.get(), i) == SQLITE_NULL) {
            row.push_back(std::nullopt);
        } else {
            const char* text = reinterpret_cast<const char*>(
                sqlite3_column_text(stmt_.get(), i));
            if (text) {
                row.push_back(std::string(text));
            } else {
                row.push_back(std::nullopt);
            }
        }
    }

    step();  // Move to next row
    return row;
}

} // namespace glz_sqlgen::sqlite

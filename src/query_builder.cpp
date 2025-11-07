#include "glz_sqlgen/core.hpp"
#include <sstream>
#include <algorithm>

namespace glz_sqlgen {

QueryBuilder& QueryBuilder::select(const std::vector<std::string>& columns) {
    columns_ = columns;
    return *this;
}

QueryBuilder& QueryBuilder::from(const std::string& table) {
    table_ = table;
    return *this;
}

QueryBuilder& QueryBuilder::where(const std::string& condition) {
    where_clause_ = condition;
    return *this;
}

std::string QueryBuilder::build() const {
    std::ostringstream query;

    // SELECT clause
    query << "SELECT ";
    if (columns_.empty()) {
        query << "*";
    } else {
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) query << ", ";
            query << columns_[i];
        }
    }

    // FROM clause
    if (!table_.empty()) {
        query << " FROM " << table_;
    }

    // WHERE clause
    if (!where_clause_.empty()) {
        query << " WHERE " << where_clause_;
    }

    return query.str();
}

void QueryBuilder::reset() {
    columns_.clear();
    table_.clear();
    where_clause_.clear();
}

} // namespace glz_sqlgen

#pragma once

#include <string>
#include <string_view>

namespace glz_sqlgen::transpilation {

/// Quote a SQL identifier (table or column name)
inline std::string quote_identifier(std::string_view identifier) {
    return "\"" + std::string(identifier) + "\"";
}

/// Escape and quote a string value for SQL
inline std::string quote_string(std::string_view value) {
    std::string result = "'";
    for (char c : value) {
        if (c == '\'') {
            result += "''"; // SQL escape for single quote
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

} // namespace glz_sqlgen::transpilation

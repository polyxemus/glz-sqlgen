#pragma once

#include "sqlite/Connection.hpp"
#include "sqlite/Iterator.hpp"
#include "Result.hpp"

namespace glz_sqlgen::sqlite {

/// Convenience function to connect to SQLite database
/// Usage: auto conn = glz_sqlgen::sqlite::connect(":memory:");
inline Result<Connection> connect(const std::string& filename = ":memory:") {
    return Connection::connect(filename);
}

} // namespace glz_sqlgen::sqlite

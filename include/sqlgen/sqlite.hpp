#pragma once

#include "sqlite/Connection.hpp"
#include "sqlite/Iterator.hpp"
#include "core.hpp"

namespace sqlgen::sqlite {

/// Convenience function to connect to SQLite database
/// Usage: auto conn = sqlgen::sqlite::connect(":memory:");
inline Result<Connection> connect(const std::string& filename = ":memory:") {
    return Connection::connect(filename);
}

} // namespace sqlgen::sqlite

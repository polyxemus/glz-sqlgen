#pragma once

#include <expected>
#include <string>
#include "common_types.hpp"

namespace glz_sqlgen {

/// Error type for database operations
using Error = std::string;

/// Result type using C++23 std::expected
template <class T>
using Result = std::expected<T, Error>;

/// Helper to create an error result
inline auto error(const std::string& msg) {
    return std::unexpected(Error{msg});
}

} // namespace glz_sqlgen

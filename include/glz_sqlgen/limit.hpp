#pragma once

#include <cstddef>
#include <optional>

namespace glz_sqlgen {

/// Represents LIMIT and optional OFFSET
struct Limit {
    size_t limit_value;
    std::optional<size_t> offset_value;
};

/// Create a LIMIT clause
inline auto limit(size_t val) {
    return Limit{.limit_value = val, .offset_value = std::nullopt};
}

/// Create a LIMIT with OFFSET clause
inline auto limit(size_t limit_val, size_t offset_val) {
    return Limit{.limit_value = limit_val, .offset_value = offset_val};
}

} // namespace glz_sqlgen

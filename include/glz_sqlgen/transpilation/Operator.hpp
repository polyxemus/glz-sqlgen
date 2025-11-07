#pragma once

namespace glz_sqlgen::transpilation {

/// SQL operators for expressions
enum class Operator {
    // Comparison
    equal,
    not_equal,
    less_than,
    less_equal,
    greater_than,
    greater_equal,

    // Arithmetic
    plus,
    minus,
    multiplies,
    divides,
    mod,

    // Special
    date_plus_duration,
    date_minus_duration,

    // Logical (for combining conditions)
    logical_and,
    logical_or,
    logical_not,

    // Pattern matching
    like,
    not_like,
    ilike,           // Case-insensitive LIKE (PostgreSQL)
    not_ilike,

    // Set operations
    in,
    not_in,

    // NULL checks
    is_null,
    is_not_null,

    // Range
    between,
    not_between
};

} // namespace glz_sqlgen::transpilation

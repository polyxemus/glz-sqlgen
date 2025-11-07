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
    logical_not
};

} // namespace glz_sqlgen::transpilation

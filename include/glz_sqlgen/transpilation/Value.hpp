#pragma once

#include <string>
#include <variant>
#include <optional>
#include <vector>
#include <cstdint>

namespace glz_sqlgen::transpilation {

/// Represents a value in SQL (parameter, literal, etc.)
/// This will be used for parameterized queries
template <class T>
struct Value {
    T val;

    constexpr Value(const T& v) : val(v) {}
    constexpr Value(T&& v) : val(std::move(v)) {}

    const T& get() const noexcept { return val; }
    T& get() noexcept { return val; }
};

// Common value types
using IntValue = Value<int64_t>;
using DoubleValue = Value<double>;
using StringValue = Value<std::string>;
using BoolValue = Value<bool>;

} // namespace glz_sqlgen::transpilation

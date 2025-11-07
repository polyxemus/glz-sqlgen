#pragma once

#include <string>
#include <type_traits>
#include "Value.hpp"

namespace glz_sqlgen::transpilation {

/// Trait to convert user-facing types to transpilation types
template <class T>
struct ToTranspilationType {
    using Type = Value<T>;

    constexpr Type operator()(const T& val) const noexcept {
        return Value<T>{val};
    }
};

/// Specialize for const char* to convert to std::string
template <>
struct ToTranspilationType<const char*> {
    using Type = Value<std::string>;

    Type operator()(const char* val) const noexcept {
        return Value<std::string>{std::string(val)};
    }
};

/// Specialize for char arrays to convert to std::string
template <int Length>
struct ToTranspilationType<char[Length]> {
    using Type = Value<std::string>;

    Type operator()(const char* val) const noexcept {
        return Value<std::string>{std::string(val)};
    }
};

/// Helper function to convert a value to its transpilation type
template <class T>
constexpr auto to_transpilation_type(const T& t) {
    return ToTranspilationType<std::remove_cvref_t<T>>{}(t);
}

/// Overload for const char* to ensure proper conversion
inline auto to_transpilation_type(const char* t) {
    return ToTranspilationType<const char*>{}(t);
}

} // namespace glz_sqlgen::transpilation

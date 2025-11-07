#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <cstdint>
#include <optional>

namespace glz_sqlgen::transpilation {

/// Map C++ types to SQL types
template <class T>
constexpr std::string_view to_sql_type() {
    using Type = std::remove_cvref_t<T>;

    if constexpr (std::is_same_v<Type, int8_t> || std::is_same_v<Type, int16_t> ||
                  std::is_same_v<Type, int32_t> || std::is_same_v<Type, int64_t> ||
                  std::is_same_v<Type, uint8_t> || std::is_same_v<Type, uint16_t> ||
                  std::is_same_v<Type, uint32_t>) {
        return "INTEGER";
    } else if constexpr (std::is_same_v<Type, uint64_t>) {
        return "BIGINT";
    } else if constexpr (std::is_same_v<Type, float> || std::is_same_v<Type, double>) {
        return "REAL";
    } else if constexpr (std::is_same_v<Type, bool>) {
        return "INTEGER"; // SQLite doesn't have BOOLEAN
    } else if constexpr (std::is_same_v<Type, std::string> || std::is_same_v<Type, std::string_view>) {
        return "TEXT";
    } else if constexpr (std::is_same_v<Type, int>) {
        return "INTEGER";
    } else {
        // For optional, extract inner type
        if constexpr (requires { typename Type::value_type; }) {
            return to_sql_type<typename Type::value_type>();
        } else {
            return "TEXT"; // Default fallback
        }
    }
}

/// Check if a type is nullable (std::optional)
template <class T>
struct is_optional : std::false_type {};

template <class T>
struct is_optional<std::optional<T>> : std::true_type {};

template <class T>
inline constexpr bool is_optional_v = is_optional<std::remove_cvref_t<T>>::value;

} // namespace glz_sqlgen::transpilation

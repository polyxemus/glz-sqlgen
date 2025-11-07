#pragma once

#include <type_traits>
#include <string>
#include <glaze/util/string_literal.hpp>

// Forward declare constraint types for traits
// (actual definitions are in wrapper_types.hpp and domain_types.hpp)
namespace glz_sqlgen {
    // Wrapper types
    template <class T, bool AutoIncr> struct PrimaryKey;
    template <class T> struct Unique;
    template <class T> struct NotNull;

    enum class ReferentialAction;
    template <class T, class RefTable, glz::string_literal column,
              ReferentialAction OnDelete, ReferentialAction OnUpdate>
    struct ForeignKey;

    template <size_t N> struct Varchar;
    template <size_t N> struct Char;
}

namespace glz_sqlgen::constraints {

// ============================================================================
// Primary Key Detection
// ============================================================================

template <class T>
struct is_primary_key : std::false_type {};

template <class T, bool AutoIncr>
struct is_primary_key<PrimaryKey<T, AutoIncr>> : std::true_type {};

template <class T>
inline constexpr bool is_primary_key_v = is_primary_key<std::remove_cvref_t<T>>::value;

// Extract auto-increment flag
template <class T>
struct is_auto_increment : std::false_type {};

template <class T, bool AutoIncr>
struct is_auto_increment<PrimaryKey<T, AutoIncr>> : std::bool_constant<AutoIncr> {};

template <class T>
inline constexpr bool is_auto_increment_v = is_auto_increment<std::remove_cvref_t<T>>::value;

// ============================================================================
// Unique Detection
// ============================================================================

template <class T>
struct is_unique : std::false_type {};

template <class T>
struct is_unique<Unique<T>> : std::true_type {};

template <class T>
inline constexpr bool is_unique_v = is_unique<std::remove_cvref_t<T>>::value;

// ============================================================================
// NotNull Detection
// ============================================================================

template <class T>
struct is_not_null : std::false_type {};

template <class T>
struct is_not_null<NotNull<T>> : std::true_type {};

template <class T>
inline constexpr bool is_not_null_v = is_not_null<std::remove_cvref_t<T>>::value;

// ============================================================================
// Foreign Key Detection
// ============================================================================

template <class T>
struct is_foreign_key : std::false_type {};

template <class T, class RefTable, glz::string_literal column, glz_sqlgen::ReferentialAction OnDelete, glz_sqlgen::ReferentialAction OnUpdate>
struct is_foreign_key<glz_sqlgen::ForeignKey<T, RefTable, column, OnDelete, OnUpdate>> : std::true_type {};

template <class T>
inline constexpr bool is_foreign_key_v = is_foreign_key<std::remove_cvref_t<T>>::value;

// Extract foreign key metadata
template <class T>
struct foreign_key_info {
    using referenced_table = void;
    static constexpr glz::string_literal referenced_column = "";
};

template <class T, class RefTable, glz::string_literal column, glz_sqlgen::ReferentialAction OnDelete, glz_sqlgen::ReferentialAction OnUpdate>
struct foreign_key_info<glz_sqlgen::ForeignKey<T, RefTable, column, OnDelete, OnUpdate>> {
    using referenced_table = RefTable;
    static constexpr auto referenced_column = column;
    static constexpr auto on_delete = OnDelete;
    static constexpr auto on_update = OnUpdate;
};

// ============================================================================
// Varchar Detection
// ============================================================================

template <class T>
struct is_varchar : std::false_type {};

template <size_t N>
struct is_varchar<glz_sqlgen::Varchar<N>> : std::true_type {};

template <class T>
inline constexpr bool is_varchar_v = is_varchar<std::remove_cvref_t<T>>::value;

template <class T>
struct varchar_length {
    static constexpr size_t value = 0;
};

template <size_t N>
struct varchar_length<glz_sqlgen::Varchar<N>> {
    static constexpr size_t value = N;
};

template <class T>
inline constexpr size_t varchar_length_v = varchar_length<std::remove_cvref_t<T>>::value;

// ============================================================================
// Char Detection
// ============================================================================

template <class T>
struct is_char : std::false_type {};

template <size_t N>
struct is_char<glz_sqlgen::Char<N>> : std::true_type {};

template <class T>
inline constexpr bool is_char_v = is_char<std::remove_cvref_t<T>>::value;

template <class T>
struct char_length {
    static constexpr size_t value = 0;
};

template <size_t N>
struct char_length<glz_sqlgen::Char<N>> {
    static constexpr size_t value = N;
};

template <class T>
inline constexpr size_t char_length_v = char_length<std::remove_cvref_t<T>>::value;

// ============================================================================
// Underlying Type Extraction
// ============================================================================

template <class T>
struct underlying_type {
    using type = T;
};

template <class T, bool AutoIncr>
struct underlying_type<PrimaryKey<T, AutoIncr>> {
    using type = T;
};

template <class T>
struct underlying_type<Unique<T>> {
    using type = T;
};

template <class T>
struct underlying_type<NotNull<T>> {
    using type = T;
};

template <class T, class RefTable, glz::string_literal column, glz_sqlgen::ReferentialAction OnDelete, glz_sqlgen::ReferentialAction OnUpdate>
struct underlying_type<glz_sqlgen::ForeignKey<T, RefTable, column, OnDelete, OnUpdate>> {
    using type = T;
};

template <size_t N>
struct underlying_type<glz_sqlgen::Varchar<N>> {
    using type = std::string;
};

template <size_t N>
struct underlying_type<glz_sqlgen::Char<N>> {
    using type = std::string;
};

template <class T>
using underlying_type_t = typename underlying_type<std::remove_cvref_t<T>>::type;

// ============================================================================
// Constraint Wrapper Detection (any constraint wrapper)
// ============================================================================

template <class T>
inline constexpr bool is_constraint_wrapper_v =
    is_primary_key_v<T> ||
    is_unique_v<T> ||
    is_not_null_v<T> ||
    is_foreign_key_v<T> ||
    is_varchar_v<T> ||
    is_char_v<T>;

} // namespace glz_sqlgen::constraints

#pragma once

#include <type_traits>
#include <utility>
#include <glaze/util/string_literal.hpp>

namespace glz_sqlgen {

/// ON DELETE/ON UPDATE actions for foreign keys
enum class ReferentialAction {
    NO_ACTION,
    RESTRICT,
    CASCADE,
    SET_NULL,
    SET_DEFAULT
};

/// Foreign key wrapper type with compile-time table and column references
/// @tparam T Underlying type (must match referenced column type)
/// @tparam RefTable Referenced table type (for compile-time validation)
/// @tparam column Referenced column name as string literal
/// @tparam OnDelete ON DELETE action (default: NO_ACTION)
/// @tparam OnUpdate ON UPDATE action (default: NO_ACTION)
template <class T,
          class RefTable,
          glz::string_literal column,
          ReferentialAction OnDelete = ReferentialAction::NO_ACTION,
          ReferentialAction OnUpdate = ReferentialAction::NO_ACTION>
struct ForeignKey {
    using value_type = T;
    using ReflectionType = T;
    using referenced_table = RefTable;

    static constexpr bool is_foreign_key = true;
    static constexpr auto referenced_column = column;
    static constexpr ReferentialAction on_delete = OnDelete;
    static constexpr ReferentialAction on_update = OnUpdate;

    T value_{};

    // Constructors
    constexpr ForeignKey() = default;
    constexpr explicit ForeignKey(const T& val) : value_(val) {}
    constexpr explicit ForeignKey(T&& val) : value_(std::move(val)) {}

    // Value access
    constexpr T& get() noexcept { return value_; }
    constexpr const T& get() const noexcept { return value_; }
    constexpr T& operator()() noexcept { return value_; }
    constexpr const T& operator()() const noexcept { return value_; }

    // Assignment
    constexpr ForeignKey& operator=(const T& val) {
        value_ = val;
        return *this;
    }

    constexpr ForeignKey& operator=(T&& val) {
        value_ = std::move(val);
        return *this;
    }

    // Conversion operator
    constexpr operator const T&() const noexcept { return value_; }
    constexpr operator T&() noexcept { return value_; }

    // Comparison operators
    constexpr bool operator==(const ForeignKey& other) const {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const ForeignKey& other) const {
        return value_ != other.value_;
    }

    constexpr bool operator<(const ForeignKey& other) const requires std::totally_ordered<T> {
        return value_ < other.value_;
    }

    constexpr bool operator<=(const ForeignKey& other) const requires std::totally_ordered<T> {
        return value_ <= other.value_;
    }

    constexpr bool operator>(const ForeignKey& other) const requires std::totally_ordered<T> {
        return value_ > other.value_;
    }

    constexpr bool operator>=(const ForeignKey& other) const requires std::totally_ordered<T> {
        return value_ >= other.value_;
    }
};

// Helper to convert ReferentialAction to SQL string
constexpr std::string_view referential_action_to_sql(ReferentialAction action) {
    switch (action) {
        case ReferentialAction::NO_ACTION: return "NO ACTION";
        case ReferentialAction::RESTRICT: return "RESTRICT";
        case ReferentialAction::CASCADE: return "CASCADE";
        case ReferentialAction::SET_NULL: return "SET NULL";
        case ReferentialAction::SET_DEFAULT: return "SET DEFAULT";
    }
    return "NO ACTION";
}

} // namespace glz_sqlgen

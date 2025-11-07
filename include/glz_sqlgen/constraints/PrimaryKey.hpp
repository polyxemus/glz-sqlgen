#pragma once

#include <type_traits>
#include <utility>
#include <optional>

namespace glz_sqlgen {

/// Primary key wrapper type
/// @tparam T Underlying type (int, int64_t, std::string, etc.)
/// @tparam AutoIncr Whether this is an auto-incrementing primary key
template <class T, bool AutoIncr = false>
struct PrimaryKey {
    using value_type = T;
    using ReflectionType = T;  // For glaze reflection

    static constexpr bool is_primary_key = true;
    static constexpr bool is_auto_increment = AutoIncr;

    // Compile-time validation
    static_assert(!std::is_same_v<T, std::optional<decltype(std::declval<T>())>>,
                  "Primary key cannot be optional/nullable");
    static_assert(!AutoIncr || std::is_integral_v<T>,
                  "Auto-increment primary key must be integral type (int, int64_t, etc.)");

    T value_{};

    // Constructors
    constexpr PrimaryKey() = default;
    constexpr explicit PrimaryKey(const T& val) : value_(val) {}
    constexpr explicit PrimaryKey(T&& val) : value_(std::move(val)) {}

    // Value access
    constexpr T& get() noexcept { return value_; }
    constexpr const T& get() const noexcept { return value_; }
    constexpr T& operator()() noexcept { return value_; }
    constexpr const T& operator()() const noexcept { return value_; }

    // Assignment
    constexpr PrimaryKey& operator=(const T& val) {
        value_ = val;
        return *this;
    }

    constexpr PrimaryKey& operator=(T&& val) {
        value_ = std::move(val);
        return *this;
    }

    // Conversion operator for convenience
    constexpr operator const T&() const noexcept { return value_; }
    constexpr operator T&() noexcept { return value_; }

    // Comparison operators (delegate to underlying type)
    constexpr bool operator==(const PrimaryKey& other) const {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const PrimaryKey& other) const {
        return value_ != other.value_;
    }

    constexpr bool operator<(const PrimaryKey& other) const requires std::totally_ordered<T> {
        return value_ < other.value_;
    }

    constexpr bool operator<=(const PrimaryKey& other) const requires std::totally_ordered<T> {
        return value_ <= other.value_;
    }

    constexpr bool operator>(const PrimaryKey& other) const requires std::totally_ordered<T> {
        return value_ > other.value_;
    }

    constexpr bool operator>=(const PrimaryKey& other) const requires std::totally_ordered<T> {
        return value_ >= other.value_;
    }
};

// Deduction guide
template <class T>
PrimaryKey(T) -> PrimaryKey<T, false>;

} // namespace glz_sqlgen

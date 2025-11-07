#pragma once

#include <type_traits>
#include <utility>
#include <optional>

namespace glz_sqlgen {

/// NOT NULL constraint wrapper type
/// Explicitly marks a field as non-nullable
/// @tparam T Underlying type
template <class T>
struct NotNull {
    using value_type = T;
    using ReflectionType = T;

    static constexpr bool is_not_null = true;

    // Compile-time validation
    static_assert(!std::is_same_v<T, std::optional<decltype(std::declval<T>())>>,
                  "NotNull wrapper cannot be applied to optional types");

    T value_{};

    // Constructors
    constexpr NotNull() = default;
    constexpr explicit NotNull(const T& val) : value_(val) {}
    constexpr explicit NotNull(T&& val) : value_(std::move(val)) {}

    // Value access
    constexpr T& get() noexcept { return value_; }
    constexpr const T& get() const noexcept { return value_; }
    constexpr T& operator()() noexcept { return value_; }
    constexpr const T& operator()() const noexcept { return value_; }

    // Assignment
    constexpr NotNull& operator=(const T& val) {
        value_ = val;
        return *this;
    }

    constexpr NotNull& operator=(T&& val) {
        value_ = std::move(val);
        return *this;
    }

    // Conversion operator
    constexpr operator const T&() const noexcept { return value_; }
    constexpr operator T&() noexcept { return value_; }

    // Comparison operators
    constexpr bool operator==(const NotNull& other) const {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const NotNull& other) const {
        return value_ != other.value_;
    }

    constexpr bool operator<(const NotNull& other) const requires std::totally_ordered<T> {
        return value_ < other.value_;
    }

    constexpr bool operator<=(const NotNull& other) const requires std::totally_ordered<T> {
        return value_ <= other.value_;
    }

    constexpr bool operator>(const NotNull& other) const requires std::totally_ordered<T> {
        return value_ > other.value_;
    }

    constexpr bool operator>=(const NotNull& other) const requires std::totally_ordered<T> {
        return value_ >= other.value_;
    }
};

// Deduction guide
template <class T>
NotNull(T) -> NotNull<T>;

} // namespace glz_sqlgen

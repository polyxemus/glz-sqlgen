#pragma once

#include <type_traits>
#include <utility>

namespace glz_sqlgen {

/// Unique constraint wrapper type
/// Marks a field as having a UNIQUE constraint
/// @tparam T Underlying type
template <class T>
struct Unique {
    using value_type = T;
    using ReflectionType = T;

    static constexpr bool is_unique = true;

    T value_{};

    // Constructors
    constexpr Unique() = default;
    constexpr explicit Unique(const T& val) : value_(val) {}
    constexpr explicit Unique(T&& val) : value_(std::move(val)) {}

    // Value access
    constexpr T& get() noexcept { return value_; }
    constexpr const T& get() const noexcept { return value_; }
    constexpr T& operator()() noexcept { return value_; }
    constexpr const T& operator()() const noexcept { return value_; }

    // Assignment
    constexpr Unique& operator=(const T& val) {
        value_ = val;
        return *this;
    }

    constexpr Unique& operator=(T&& val) {
        value_ = std::move(val);
        return *this;
    }

    // Conversion operator
    constexpr operator const T&() const noexcept { return value_; }
    constexpr operator T&() noexcept { return value_; }

    // Comparison operators
    constexpr bool operator==(const Unique& other) const {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const Unique& other) const {
        return value_ != other.value_;
    }

    constexpr bool operator<(const Unique& other) const requires std::totally_ordered<T> {
        return value_ < other.value_;
    }

    constexpr bool operator<=(const Unique& other) const requires std::totally_ordered<T> {
        return value_ <= other.value_;
    }

    constexpr bool operator>(const Unique& other) const requires std::totally_ordered<T> {
        return value_ > other.value_;
    }

    constexpr bool operator>=(const Unique& other) const requires std::totally_ordered<T> {
        return value_ >= other.value_;
    }
};

// Deduction guide
template <class T>
Unique(T) -> Unique<T>;

} // namespace glz_sqlgen

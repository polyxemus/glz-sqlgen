#pragma once

#include <type_traits>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>

namespace glz_sqlgen {

/// Variable-length string with maximum length constraint
/// Enforces length validation at runtime
/// @tparam N Maximum length (must be > 0)
template <size_t N>
struct Varchar {
    using value_type = std::string;
    using ReflectionType = std::string;

    static constexpr bool is_varchar = true;
    static constexpr size_t max_length = N;

    // Compile-time validation
    static_assert(N > 0, "Varchar length must be greater than 0");

    std::string value_{};

    // Constructors
    constexpr Varchar() = default;

    explicit Varchar(const std::string& val) : value_(val) {
        validate();
    }

    explicit Varchar(std::string&& val) : value_(std::move(val)) {
        validate();
    }

    explicit Varchar(const char* val) : value_(val) {
        validate();
    }

    explicit Varchar(std::string_view val) : value_(val) {
        validate();
    }

    // Value access
    std::string& get() noexcept { return value_; }
    const std::string& get() const noexcept { return value_; }
    std::string& operator()() noexcept { return value_; }
    const std::string& operator()() const noexcept { return value_; }

    // Assignment with validation
    Varchar& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    Varchar& operator=(std::string&& val) {
        value_ = std::move(val);
        validate();
        return *this;
    }

    Varchar& operator=(const char* val) {
        value_ = val;
        validate();
        return *this;
    }

    Varchar& operator=(std::string_view val) {
        value_ = std::string(val);
        validate();
        return *this;
    }

    // Conversion operator
    operator const std::string&() const noexcept { return value_; }
    operator std::string&() noexcept { return value_; }

    // Comparison operators
    bool operator==(const Varchar& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Varchar& other) const {
        return value_ != other.value_;
    }

    bool operator<(const Varchar& other) const {
        return value_ < other.value_;
    }

    bool operator<=(const Varchar& other) const {
        return value_ <= other.value_;
    }

    bool operator>(const Varchar& other) const {
        return value_ > other.value_;
    }

    bool operator>=(const Varchar& other) const {
        return value_ >= other.value_;
    }

    // String operations
    size_t size() const noexcept { return value_.size(); }
    size_t length() const noexcept { return value_.length(); }
    bool empty() const noexcept { return value_.empty(); }
    void clear() noexcept { value_.clear(); }

    const char* c_str() const noexcept { return value_.c_str(); }
    const char* data() const noexcept { return value_.data(); }

private:
    void validate() const {
        if (value_.length() > N) {
            throw std::length_error(
                "Varchar value exceeds maximum length of " + std::to_string(N) +
                " (got " + std::to_string(value_.length()) + ")"
            );
        }
    }
};

} // namespace glz_sqlgen

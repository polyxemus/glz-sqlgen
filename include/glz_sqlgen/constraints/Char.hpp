#pragma once

#include <type_traits>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>

namespace glz_sqlgen {

/// Fixed-length string with padding
/// Enforces exact length by padding with spaces if necessary
/// @tparam N Fixed length (must be > 0)
template <size_t N>
struct Char {
    using value_type = std::string;
    using ReflectionType = std::string;

    static constexpr bool is_char = true;
    static constexpr size_t fixed_length = N;

    // Compile-time validation
    static_assert(N > 0, "Char length must be greater than 0");

    std::string value_{};

    // Constructors
    constexpr Char() : value_(N, ' ') {}

    explicit Char(const std::string& val) : value_(val) {
        normalize();
    }

    explicit Char(std::string&& val) : value_(std::move(val)) {
        normalize();
    }

    explicit Char(const char* val) : value_(val) {
        normalize();
    }

    explicit Char(std::string_view val) : value_(val) {
        normalize();
    }

    // Value access (returns trimmed value)
    std::string get() const noexcept {
        // Return value with trailing spaces trimmed for user convenience
        auto end = value_.find_last_not_of(' ');
        if (end == std::string::npos) {
            return "";
        }
        return value_.substr(0, end + 1);
    }

    // Get raw padded value
    const std::string& get_raw() const noexcept { return value_; }
    std::string& get_raw() noexcept { return value_; }

    std::string operator()() const noexcept { return get(); }

    // Assignment with normalization
    Char& operator=(const std::string& val) {
        value_ = val;
        normalize();
        return *this;
    }

    Char& operator=(std::string&& val) {
        value_ = std::move(val);
        normalize();
        return *this;
    }

    Char& operator=(const char* val) {
        value_ = val;
        normalize();
        return *this;
    }

    Char& operator=(std::string_view val) {
        value_ = std::string(val);
        normalize();
        return *this;
    }

    // Conversion operator (returns trimmed value)
    operator std::string() const noexcept { return get(); }

    // Comparison operators (compare raw values)
    bool operator==(const Char& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Char& other) const {
        return value_ != other.value_;
    }

    bool operator<(const Char& other) const {
        return value_ < other.value_;
    }

    bool operator<=(const Char& other) const {
        return value_ <= other.value_;
    }

    bool operator>(const Char& other) const {
        return value_ > other.value_;
    }

    bool operator>=(const Char& other) const {
        return value_ >= other.value_;
    }

    // String operations
    size_t size() const noexcept { return N; }
    size_t length() const noexcept { return N; }
    constexpr bool empty() const noexcept { return N == 0; }

    const char* c_str() const noexcept { return value_.c_str(); }
    const char* data() const noexcept { return value_.data(); }

private:
    void normalize() {
        if (value_.length() > N) {
            throw std::length_error(
                "Char value exceeds fixed length of " + std::to_string(N) +
                " (got " + std::to_string(value_.length()) + ")"
            );
        }
        // Pad with spaces to reach fixed length
        if (value_.length() < N) {
            value_.resize(N, ' ');
        }
    }
};

} // namespace glz_sqlgen

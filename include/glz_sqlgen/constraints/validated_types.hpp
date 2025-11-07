#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <regex>
#include <cctype>
#include <algorithm>

namespace glz_sqlgen {

// ============================================================================
// AlphaNumeric - Alphanumeric validation (letters, numbers, underscore)
// ============================================================================

/// String validated to contain only alphanumeric characters and underscores
struct AlphaNumeric {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    AlphaNumeric() = default;
    explicit AlphaNumeric(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    AlphaNumeric& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const AlphaNumeric&) const = default;

private:
    void validate() const {
        if (value_.empty()) {
            throw std::invalid_argument("AlphaNumeric value cannot be empty");
        }
        for (char c : value_) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                throw std::invalid_argument("AlphaNumeric value must contain only letters, numbers, and underscores");
            }
        }
    }
};

// ============================================================================
// Base64Encoded - Base64 validation
// ============================================================================

/// String validated to be valid Base64 encoding
struct Base64Encoded {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    Base64Encoded() = default;
    explicit Base64Encoded(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    Base64Encoded& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const Base64Encoded&) const = default;

private:
    void validate() const {
        if (value_.empty()) return;  // Empty is valid

        // Base64 uses A-Z, a-z, 0-9, +, /, and = for padding
        for (size_t i = 0; i < value_.length(); ++i) {
            char c = value_[i];
            bool valid = (c >= 'A' && c <= 'Z') ||
                        (c >= 'a' && c <= 'z') ||
                        (c >= '0' && c <= '9') ||
                        c == '+' || c == '/' || c == '=';
            if (!valid) {
                throw std::invalid_argument("Invalid Base64 character");
            }
            // Padding (=) should only appear at the end
            if (c == '=' && i < value_.length() - 2) {
                throw std::invalid_argument("Base64 padding can only appear at the end");
            }
        }

        // Length should be multiple of 4
        if (value_.length() % 4 != 0) {
            throw std::invalid_argument("Base64 string length must be a multiple of 4");
        }
    }
};

// ============================================================================
// Email - Email validation
// ============================================================================

/// String validated to be a valid email address
struct Email {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    Email() = default;
    explicit Email(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    Email& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const Email&) const = default;

private:
    void validate() const {
        if (value_.empty()) {
            throw std::invalid_argument("Email cannot be empty");
        }

        // Simple email validation: local@domain
        // RFC 5322 is complex, this is a practical approximation
        static const std::regex email_pattern(
            R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );

        if (!std::regex_match(value_, email_pattern)) {
            throw std::invalid_argument("Invalid email format");
        }
    }
};

// ============================================================================
// UUIDv4 - UUID version 4 validation (random UUID)
// ============================================================================

/// UUID validated to be version 4 format
struct UUIDv4 {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    UUIDv4() = default;
    explicit UUIDv4(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    UUIDv4& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const UUIDv4&) const = default;

private:
    void validate() const {
        if (value_.length() != 36) {
            throw std::invalid_argument("UUIDv4 must be 36 characters long");
        }

        // Format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
        // where y is 8, 9, a, or b
        if (value_[8] != '-' || value_[13] != '-' || value_[18] != '-' || value_[23] != '-') {
            throw std::invalid_argument("Invalid UUIDv4 format");
        }

        // Check version (character at position 14 should be '4')
        if (value_[14] != '4') {
            throw std::invalid_argument("Not a valid UUIDv4 (version must be 4)");
        }

        // Check variant (character at position 19 should be 8, 9, a, or b)
        char variant = std::tolower(static_cast<unsigned char>(value_[19]));
        if (variant != '8' && variant != '9' && variant != 'a' && variant != 'b') {
            throw std::invalid_argument("Not a valid UUIDv4 (invalid variant)");
        }

        // Validate all hex characters
        for (size_t i = 0; i < value_.length(); ++i) {
            if (i == 8 || i == 13 || i == 18 || i == 23) continue;
            char c = value_[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                throw std::invalid_argument("UUIDv4 must contain only hexadecimal characters");
            }
        }
    }
};

// ============================================================================
// URL-safe variants
// ============================================================================

/// String validated to contain only URL-safe characters
struct UrlSafe {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    UrlSafe() = default;
    explicit UrlSafe(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    UrlSafe& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const UrlSafe&) const = default;

private:
    void validate() const {
        // URL-safe: alphanumeric, dash, underscore, period, tilde
        for (char c : value_) {
            bool valid = std::isalnum(static_cast<unsigned char>(c)) ||
                        c == '-' || c == '_' || c == '.' || c == '~';
            if (!valid) {
                throw std::invalid_argument("UrlSafe value contains invalid characters");
            }
        }
    }
};

// ============================================================================
// Hex - Hexadecimal string validation
// ============================================================================

/// String validated to contain only hexadecimal characters
struct Hex {
    using ReflectionType = std::string;
    static constexpr bool is_validated = true;

    std::string value_{};

    Hex() = default;
    explicit Hex(const std::string& val) : value_(val) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    Hex& operator=(const std::string& val) {
        value_ = val;
        validate();
        return *this;
    }

    operator const std::string&() const noexcept { return value_; }

    auto operator<=>(const Hex&) const = default;

private:
    void validate() const {
        if (value_.empty()) {
            throw std::invalid_argument("Hex value cannot be empty");
        }
        for (char c : value_) {
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                throw std::invalid_argument("Hex value must contain only hexadecimal characters");
            }
        }
    }
};

} // namespace glz_sqlgen

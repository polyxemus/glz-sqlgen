#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <glaze/glaze.hpp>

namespace glz_sqlgen {

// ============================================================================
// JSON - JSON field storage with glaze serialization
// ============================================================================

/// JSON field type with automatic serialization/deserialization using glaze
/// @tparam T The type to serialize/deserialize
template <class T>
struct JSON {
    using value_type = T;
    using ReflectionType = std::string;  // Stored as string in DB

    static constexpr bool is_json = true;

    T value_{};

    // Constructors
    JSON() = default;
    explicit JSON(const T& val) : value_(val) {}
    explicit JSON(T&& val) : value_(std::move(val)) {}

    // Construct from JSON string
    explicit JSON(const std::string& json_str) {
        auto result = glz::read_json(value_, json_str);
        if (result) {
            throw std::runtime_error("Failed to parse JSON: " + std::string(glz::format_error(result, json_str)));
        }
    }

    // Value access
    T& get() noexcept { return value_; }
    const T& get() const noexcept { return value_; }
    T& operator()() noexcept { return value_; }
    const T& operator()() const noexcept { return value_; }

    // Assignment
    JSON& operator=(const T& val) {
        value_ = val;
        return *this;
    }

    JSON& operator=(T&& val) {
        value_ = std::move(val);
        return *this;
    }

    // Conversion operator
    operator const T&() const noexcept { return value_; }
    operator T&() noexcept { return value_; }

    // Serialize to JSON string
    std::string to_json() const {
        auto result = glz::write_json(value_);
        if (result.has_value()) {
            return result.value();
        }
        return "{}";  // Return empty object on error
    }

    // Deserialize from JSON string
    void from_json(const std::string& json_str) {
        auto result = glz::read_json(value_, json_str);
        if (result) {
            throw std::runtime_error("Failed to parse JSON: " + std::string(glz::format_error(result, json_str)));
        }
    }

    // Comparison operators
    bool operator==(const JSON& other) const {
        return to_json() == other.to_json();
    }

    bool operator!=(const JSON& other) const {
        return to_json() != other.to_json();
    }
};

// ============================================================================
// Date - Date-only type (YYYY-MM-DD)
// ============================================================================

/// Date type representing a calendar date without time
struct Date {
    using ReflectionType = std::string;  // Stored as ISO 8601 date string
    static constexpr bool is_date = true;

    int year{1970};
    int month{1};
    int day{1};

    Date() = default;
    Date(int y, int m, int d) : year(y), month(m), day(d) {
        validate();
    }

    // Parse from ISO 8601 date string (YYYY-MM-DD)
    explicit Date(const std::string& date_str) {
        if (date_str.length() != 10 || date_str[4] != '-' || date_str[7] != '-') {
            throw std::invalid_argument("Invalid date format. Expected YYYY-MM-DD");
        }
        year = std::stoi(date_str.substr(0, 4));
        month = std::stoi(date_str.substr(5, 2));
        day = std::stoi(date_str.substr(8, 2));
        validate();
    }

    // Convert to ISO 8601 string
    std::string to_string() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << year << '-'
            << std::setw(2) << month << '-'
            << std::setw(2) << day;
        return oss.str();
    }

    // Comparison operators
    auto operator<=>(const Date&) const = default;

private:
    void validate() const {
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Month must be between 1 and 12");
        }
        if (day < 1 || day > 31) {
            throw std::invalid_argument("Day must be between 1 and 31");
        }
    }
};

// ============================================================================
// DateTime - Date and time type (ISO 8601)
// ============================================================================

/// DateTime type representing a date and time
struct DateTime {
    using ReflectionType = std::string;  // Stored as ISO 8601 datetime string
    static constexpr bool is_datetime = true;

    int year{1970};
    int month{1};
    int day{1};
    int hour{0};
    int minute{0};
    int second{0};

    DateTime() = default;
    DateTime(int y, int mo, int d, int h, int mi, int s)
        : year(y), month(mo), day(d), hour(h), minute(mi), second(s) {
        validate();
    }

    // Parse from ISO 8601 datetime string (YYYY-MM-DD HH:MM:SS or YYYY-MM-DDTHH:MM:SS)
    explicit DateTime(const std::string& datetime_str) {
        size_t pos = datetime_str.find('T');
        if (pos == std::string::npos) {
            pos = datetime_str.find(' ');
        }
        if (pos == std::string::npos || datetime_str.length() < 19) {
            throw std::invalid_argument("Invalid datetime format. Expected YYYY-MM-DD HH:MM:SS or YYYY-MM-DDTHH:MM:SS");
        }

        year = std::stoi(datetime_str.substr(0, 4));
        month = std::stoi(datetime_str.substr(5, 2));
        day = std::stoi(datetime_str.substr(8, 2));
        hour = std::stoi(datetime_str.substr(pos + 1, 2));
        minute = std::stoi(datetime_str.substr(pos + 4, 2));
        second = std::stoi(datetime_str.substr(pos + 7, 2));
        validate();
    }

    // Convert to ISO 8601 string
    std::string to_string() const {
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(4) << year << '-'
            << std::setw(2) << month << '-'
            << std::setw(2) << day << ' '
            << std::setw(2) << hour << ':'
            << std::setw(2) << minute << ':'
            << std::setw(2) << second;
        return oss.str();
    }

    // Comparison operators
    auto operator<=>(const DateTime&) const = default;

private:
    void validate() const {
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Month must be between 1 and 12");
        }
        if (day < 1 || day > 31) {
            throw std::invalid_argument("Day must be between 1 and 31");
        }
        if (hour < 0 || hour > 23) {
            throw std::invalid_argument("Hour must be between 0 and 23");
        }
        if (minute < 0 || minute > 59) {
            throw std::invalid_argument("Minute must be between 0 and 59");
        }
        if (second < 0 || second > 59) {
            throw std::invalid_argument("Second must be between 0 and 59");
        }
    }
};

// ============================================================================
// Timestamp - Unix timestamp wrapper
// ============================================================================

/// Timestamp type representing seconds since Unix epoch
struct Timestamp {
    using ReflectionType = int64_t;
    static constexpr bool is_timestamp = true;

    int64_t value_{0};

    Timestamp() = default;
    explicit Timestamp(int64_t val) : value_(val) {}

    // Current timestamp
    static Timestamp now() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return Timestamp(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    }

    int64_t get() const noexcept { return value_; }
    int64_t& get() noexcept { return value_; }

    operator int64_t() const noexcept { return value_; }

    // Convert to DateTime
    DateTime to_datetime() const {
        std::time_t time = static_cast<std::time_t>(value_);
        std::tm* tm = std::gmtime(&time);
        if (!tm) {
            throw std::runtime_error("Failed to convert timestamp to datetime");
        }
        return DateTime(
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec
        );
    }

    // Comparison operators
    auto operator<=>(const Timestamp&) const = default;
};

// ============================================================================
// UUID - Universal Unique Identifier
// ============================================================================

/// UUID type with validation
struct UUID {
    using ReflectionType = std::string;
    static constexpr bool is_uuid = true;

    std::string value_{};

    UUID() = default;
    explicit UUID(const std::string& uuid_str) : value_(uuid_str) {
        validate();
    }

    const std::string& get() const noexcept { return value_; }
    std::string& get() noexcept { return value_; }

    operator const std::string&() const noexcept { return value_; }

    // Comparison operators
    auto operator<=>(const UUID&) const = default;

private:
    void validate() const {
        if (value_.length() != 36) {
            throw std::invalid_argument("UUID must be 36 characters long");
        }
        if (value_[8] != '-' || value_[13] != '-' || value_[18] != '-' || value_[23] != '-') {
            throw std::invalid_argument("Invalid UUID format. Expected 8-4-4-4-12 format");
        }
        // Validate hex characters
        for (size_t i = 0; i < value_.length(); ++i) {
            if (i == 8 || i == 13 || i == 18 || i == 23) continue;
            char c = value_[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                throw std::invalid_argument("UUID must contain only hexadecimal characters");
            }
        }
    }
};

} // namespace glz_sqlgen

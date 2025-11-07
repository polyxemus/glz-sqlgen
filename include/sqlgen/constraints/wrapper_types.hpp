#pragma once

#include <type_traits>
#include <utility>
#include <optional>
#include <string>
#include <string_view>
#include <stdexcept>
#include <glaze/util/string_literal.hpp>

namespace sqlgen {

// ============================================================================
// Metadata Structures
// ============================================================================

namespace constraints {

/// Foreign key reference information
struct ForeignKeyReference {
    std::string table;
    std::string column;
    std::string on_delete = "";  // ON DELETE action (CASCADE, SET NULL, etc.)
    std::string on_update = "";  // ON UPDATE action
};

/// Constraint metadata for a field
struct FieldConstraints {
    bool is_primary_key = false;
    bool auto_increment = false;
    bool is_unique = false;
    bool is_not_null = false;
    std::optional<ForeignKeyReference> foreign_key;
    std::optional<size_t> max_length;      // For VARCHAR(N)
    std::optional<size_t> fixed_length;    // For CHAR(N)
    std::optional<std::string> pattern;    // Regex pattern for validation
    std::optional<std::string> default_value;

    // Specialized type flags
    bool is_json = false;
    bool is_timestamp = false;
    bool is_date = false;
    bool is_datetime = false;
    bool is_uuid = false;
    std::optional<std::string> timestamp_format;
};

} // namespace constraints

// ============================================================================
// PrimaryKey - Primary key wrapper type
// ============================================================================

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

// ============================================================================
// Unique - Unique constraint wrapper type
// ============================================================================

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

// ============================================================================
// NotNull - NOT NULL constraint wrapper type
// ============================================================================

/// NOT NULL constraint wrapper type
/// Marks a field as having a NOT NULL constraint
/// @tparam T Underlying type
template <class T>
struct NotNull {
    using value_type = T;
    using ReflectionType = T;

    static constexpr bool is_not_null = true;

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

// ============================================================================
// ForeignKey - Foreign key wrapper type
// ============================================================================

/// ON DELETE/ON UPDATE actions for foreign keys
enum class ReferentialAction {
    NO_ACTION,
    RESTRICT,
    CASCADE,
    SET_NULL,
    SET_DEFAULT
};

/// Helper to convert ReferentialAction to SQL string
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

// ============================================================================
// Varchar - Variable-length string with maximum length constraint
// ============================================================================

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

// ============================================================================
// Char - Fixed-length string with padding
// ============================================================================

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

} // namespace sqlgen

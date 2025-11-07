#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <expected>
#include <concepts>
#include <sstream>
#include <glaze/util/string_literal.hpp>
#include "transpilation_core.hpp"

namespace sqlgen {

// ============================================================================
// Common Types
// ============================================================================

/// Placeholder type for "nothing specified" in query builders
struct Nothing {};

// ============================================================================
// Result Types
// ============================================================================

/// Error type for database operations
using Error = std::string;

/// Result type using C++23 std::expected
template <class T>
using Result = std::expected<T, Error>;

/// Helper to create an error result
inline auto error(const std::string& msg) {
    return std::unexpected(Error{msg});
}

// ============================================================================
// Column Types
// ============================================================================

/// User-facing column template
/// Represents a column reference that can be used in queries
template <glz::string_literal Name, glz::string_literal Alias = "">
struct Col {
    using ColType = transpilation::Col<Name, Alias>;
    static constexpr std::string_view name = Name.sv();
    static constexpr std::string_view alias = Alias.sv();

    constexpr Col() = default;

    /// Implicit conversion to transpilation Col for to_sql() calls
    constexpr operator ColType() const noexcept {
        return ColType{};
    }

    /// Create an AS alias for this column
    template <glz::string_literal NewName>
    constexpr auto as() const noexcept {
        return transpilation::make_as<ColType, NewName>(ColType{});
    }

    /// Mark this column for descending sort order
    constexpr auto desc() const noexcept {
        return transpilation::make_desc(ColType{});
    }

    /// Returns the column name
    constexpr std::string_view get_name() const noexcept {
        return name;
    }

    // Comparison operators
    template <class T>
    friend constexpr auto operator==(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator!=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::not_equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator<(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::less_than>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator<=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::less_equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator>(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::greater_than>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator>=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::greater_equal>(ColType{}, Value<T>{rhs}));
    }

    // Arithmetic operators
    template <class T>
    friend constexpr auto operator+(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::plus>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator-(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::minus>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator*(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::multiplies>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator/(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::divides>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator%(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::mod>(ColType{}, Value<T>{rhs});
    }

    // Column-to-column operations
    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator==(const Col&, const Col<OtherName, OtherAlias>&) {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_condition_wrapper(
            make_condition<Operator::equal>(ColType{}, OtherColType{}));
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator!=(const Col&, const Col<OtherName, OtherAlias>&) {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_condition_wrapper(
            make_condition<Operator::not_equal>(ColType{}, OtherColType{}));
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator+(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::plus>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator-(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::minus>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator*(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::multiplies>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator/(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::divides>(ColType{}, OtherColType{});
    }
};

/// Helper to create column references
template <glz::string_literal Name, glz::string_literal Alias = "">
inline constexpr Col<Name, Alias> col{};

// ============================================================================
// Literal Operators
// ============================================================================

namespace literals {

/// Column literal operator: "name"_c creates Col<"name">
template <glz::string_literal Name>
constexpr auto operator""_c() {
    return Col<Name>{};
}

/// Table alias literal operators: "name"_t1 creates Col<"name", "t1">
#define GLZ_SQLGEN_TABLE_ALIAS_LITERAL(N)           \
    template <glz::string_literal Name>             \
    constexpr auto operator""_t##N() {              \
        return Col<Name, "t" #N>{};                 \
    }

GLZ_SQLGEN_TABLE_ALIAS_LITERAL(1)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(2)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(3)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(4)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(5)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(6)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(7)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(8)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(9)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(10)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(11)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(12)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(13)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(14)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(15)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(16)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(17)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(18)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(19)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(20)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(21)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(22)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(23)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(24)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(25)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(26)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(27)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(28)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(29)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(30)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(31)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(32)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(33)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(34)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(35)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(36)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(37)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(38)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(39)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(40)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(41)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(42)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(43)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(44)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(45)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(46)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(47)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(48)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(49)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(50)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(51)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(52)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(53)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(54)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(55)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(56)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(57)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(58)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(59)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(60)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(61)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(62)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(63)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(64)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(65)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(66)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(67)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(68)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(69)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(70)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(71)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(72)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(73)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(74)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(75)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(76)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(77)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(78)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(79)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(80)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(81)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(82)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(83)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(84)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(85)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(86)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(87)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(88)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(89)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(90)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(91)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(92)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(93)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(94)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(95)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(96)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(97)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(98)
GLZ_SQLGEN_TABLE_ALIAS_LITERAL(99)

#undef GLZ_SQLGEN_TABLE_ALIAS_LITERAL

} // namespace literals

// ============================================================================
// Query Builder
// ============================================================================

/// A simple query builder class for demonstration purposes
class QueryBuilder {
public:
    QueryBuilder() = default;

    /// Add a SELECT clause
    QueryBuilder& select(const std::vector<std::string>& columns) {
        columns_ = columns;
        return *this;
    }

    /// Add a FROM clause
    QueryBuilder& from(const std::string& table) {
        table_ = table;
        return *this;
    }

    /// Add a WHERE clause
    QueryBuilder& where(const std::string& condition) {
        where_clause_ = condition;
        return *this;
    }

    /// Build and return the SQL query string
    [[nodiscard]] std::string build() const {
        std::ostringstream query;

        // SELECT clause
        query << "SELECT ";
        if (columns_.empty()) {
            query << "*";
        } else {
            for (size_t i = 0; i < columns_.size(); ++i) {
                if (i > 0) query << ", ";
                query << columns_[i];
            }
        }

        // FROM clause
        if (!table_.empty()) {
            query << " FROM " << table_;
        }

        // WHERE clause
        if (!where_clause_.empty()) {
            query << " WHERE " << where_clause_;
        }

        return query.str();
    }

    /// Reset the builder to empty state
    void reset() {
        columns_.clear();
        table_.clear();
        where_clause_.clear();
    }

private:
    std::vector<std::string> columns_;
    std::string table_;
    std::string where_clause_;
};

} // namespace sqlgen

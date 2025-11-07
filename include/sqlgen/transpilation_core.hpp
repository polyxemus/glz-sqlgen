#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <cstdint>
#include <optional>
#include <tuple>
#include <glaze/util/string_literal.hpp>

namespace sqlgen::transpilation {

// ============================================================================
// OPERATOR
// ============================================================================

/// SQL operators for expressions
enum class Operator {
    // Comparison
    equal,
    not_equal,
    less_than,
    less_equal,
    greater_than,
    greater_equal,

    // Arithmetic
    plus,
    minus,
    multiplies,
    divides,
    mod,

    // Special
    date_plus_duration,
    date_minus_duration,

    // Logical (for combining conditions)
    logical_and,
    logical_or,
    logical_not,

    // Pattern matching
    like,
    not_like,
    ilike,           // Case-insensitive LIKE (PostgreSQL)
    not_ilike,

    // Set operations
    in,
    not_in,

    // NULL checks
    is_null,
    is_not_null,

    // Range
    between,
    not_between
};

// ============================================================================
// QUOTING
// ============================================================================

/// Quote a SQL identifier (table or column name)
inline std::string quote_identifier(std::string_view identifier) {
    return "\"" + std::string(identifier) + "\"";
}

/// Escape and quote a string value for SQL
inline std::string quote_string(std::string_view value) {
    std::string result = "'";
    for (char c : value) {
        if (c == '\'') {
            result += "''"; // SQL escape for single quote
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

// ============================================================================
// TYPE MAPPING
// ============================================================================

/// Map C++ types to SQL types
template <class T>
constexpr std::string_view to_sql_type() {
    using Type = std::remove_cvref_t<T>;

    if constexpr (std::is_same_v<Type, int8_t> || std::is_same_v<Type, int16_t> ||
                  std::is_same_v<Type, int32_t> || std::is_same_v<Type, int64_t> ||
                  std::is_same_v<Type, uint8_t> || std::is_same_v<Type, uint16_t> ||
                  std::is_same_v<Type, uint32_t>) {
        return "INTEGER";
    } else if constexpr (std::is_same_v<Type, uint64_t>) {
        return "BIGINT";
    } else if constexpr (std::is_same_v<Type, float> || std::is_same_v<Type, double>) {
        return "REAL";
    } else if constexpr (std::is_same_v<Type, bool>) {
        return "INTEGER"; // SQLite doesn't have BOOLEAN
    } else if constexpr (std::is_same_v<Type, std::string> || std::is_same_v<Type, std::string_view>) {
        return "TEXT";
    } else if constexpr (std::is_same_v<Type, int>) {
        return "INTEGER";
    } else {
        // For optional, extract inner type
        if constexpr (requires { typename Type::value_type; }) {
            return to_sql_type<typename Type::value_type>();
        } else {
            return "TEXT"; // Default fallback
        }
    }
}

/// Check if a type is nullable (std::optional)
template <class T>
struct is_optional : std::false_type {};

template <class T>
struct is_optional<std::optional<T>> : std::true_type {};

template <class T>
inline constexpr bool is_optional_v = is_optional<std::remove_cvref_t<T>>::value;

// ============================================================================
// VALUE
// ============================================================================

/// Represents a value in SQL (parameter, literal, etc.)
/// This will be used for parameterized queries
template <class T>
struct Value {
    T val;

    constexpr Value(const T& v) : val(v) {}
    constexpr Value(T&& v) : val(std::move(v)) {}

    const T& get() const noexcept { return val; }
    T& get() noexcept { return val; }
};

/// Specialization for string literals (char arrays)
template <size_t N>
struct Value<char[N]> {
    std::string val;

    constexpr Value(const char (&str)[N]) : val(str) {}

    const std::string& get() const noexcept { return val; }
    std::string& get() noexcept { return val; }
};

// Common value types
using IntValue = Value<int64_t>;
using DoubleValue = Value<double>;
using StringValue = Value<std::string>;
using BoolValue = Value<bool>;

// ============================================================================
// COLUMN
// ============================================================================

/// Transpilation-level column representation
/// Used internally for SQL generation
template <glz::string_literal Name, glz::string_literal Alias = "">
struct Col {
    static constexpr std::string_view name = Name.sv();
    static constexpr std::string_view alias = Alias.sv();

    constexpr Col() = default;

    constexpr bool has_alias() const noexcept {
        return !alias.empty();
    }

    constexpr std::string_view get_name() const noexcept {
        return name;
    }

    constexpr std::string_view get_alias() const noexcept {
        return alias;
    }
};

// ============================================================================
// DESC, AS, SET
// ============================================================================

/// Marker for descending sort order
template <class Col>
struct Desc {
    Col column;

    constexpr Desc(const Col& c) : column(c) {}
    constexpr Desc(Col&& c) : column(std::move(c)) {}

    const Col& get_column() const noexcept { return column; }
};

template <class Col>
constexpr auto make_desc(Col&& col) {
    return Desc<std::remove_cvref_t<Col>>{std::forward<Col>(col)};
}

/// Represents column aliasing (col AS alias_name)
template <class Value, glz::string_literal AliasName>
struct As {
    Value val;
    static constexpr std::string_view alias_name = AliasName.sv();

    constexpr As(const Value& v) : val(v) {}
    constexpr As(Value&& v) : val(std::move(v)) {}

    constexpr std::string_view get_alias() const noexcept {
        return alias_name;
    }

    const Value& get_value() const noexcept { return val; }
};

template <class Value, glz::string_literal AliasName>
constexpr auto make_as(Value&& val) {
    return As<std::remove_cvref_t<Value>, AliasName>{std::forward<Value>(val)};
}

/// Represents a SET clause for UPDATE (column = value)
template <class Col, class Value>
struct Set {
    Col column;
    Value value;

    constexpr Set(const Col& c, const Value& v) : column(c), value(v) {}
    constexpr Set(Col&& c, Value&& v) : column(std::move(c)), value(std::move(v)) {}
};

template <class Col, class Value>
constexpr auto make_set(Col&& col, Value&& val) {
    return Set<std::remove_cvref_t<Col>, std::remove_cvref_t<Value>>{
        std::forward<Col>(col), std::forward<Value>(val)};
}

// ============================================================================
// OPERATION
// ============================================================================

/// Represents a binary operation (e.g., col1 + col2, col * 5)
template <Operator Op, class Operand1, class Operand2>
struct Operation {
    Operand1 operand1;
    Operand2 operand2;

    static constexpr Operator op = Op;

    constexpr Operation(const Operand1& op1, const Operand2& op2)
        : operand1(op1), operand2(op2) {}
};

/// Helper to create operations
template <Operator Op, class T1, class T2>
constexpr auto make_operation(T1&& op1, T2&& op2) {
    return Operation<Op, std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>{
        std::forward<T1>(op1), std::forward<T2>(op2)};
}

// Operators for chaining operations (arithmetic)
template <Operator Op1, class O1, class O2, class T>
constexpr auto operator+(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::plus>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator-(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::minus>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator*(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::multiplies>(lhs, Value<T>{rhs});
}

template <Operator Op1, class O1, class O2, class T>
constexpr auto operator/(const Operation<Op1, O1, O2>& lhs, const T& rhs) {
    return make_operation<Operator::divides>(lhs, Value<T>{rhs});
}

// Comparison operators for operations (to create conditions)
template <Operator Op, class O1, class O2, class T>
constexpr auto operator==(const Operation<Op, O1, O2>& lhs, const T& rhs);

template <Operator Op, class O1, class O2, class T>
constexpr auto operator!=(const Operation<Op, O1, O2>& lhs, const T& rhs);

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<(const Operation<Op, O1, O2>& lhs, const T& rhs);

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<=(const Operation<Op, O1, O2>& lhs, const T& rhs);

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>(const Operation<Op, O1, O2>& lhs, const T& rhs);

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>=(const Operation<Op, O1, O2>& lhs, const T& rhs);

// ============================================================================
// CONDITION
// ============================================================================

/// Represents a SQL condition (WHERE clause component)
template <class LeftOperand, Operator Op, class RightOperand>
struct Condition {
    LeftOperand left;
    RightOperand right;

    static constexpr Operator op = Op;

    constexpr Condition(const LeftOperand& l, const RightOperand& r)
        : left(l), right(r) {}
};

/// Helper to create conditions
template <Operator Op, class L, class R>
constexpr auto make_condition(L&& left, R&& right) {
    return Condition<std::remove_cvref_t<L>, Op, std::remove_cvref_t<R>>{
        std::forward<L>(left), std::forward<R>(right)};
}

/// Wrapper to mark a condition
template <class T>
struct ConditionWrapper {
    T condition;

    constexpr ConditionWrapper(T&& cond) : condition(std::move(cond)) {}
    constexpr ConditionWrapper(const T& cond) : condition(cond) {}
};

template <class T>
constexpr auto make_condition_wrapper(T&& cond) {
    return ConditionWrapper<std::remove_cvref_t<T>>{std::forward<T>(cond)};
}

// Comparison operators for Operations to create Conditions
template <Operator Op, class O1, class O2, class T>
constexpr auto operator==(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator!=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::not_equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::less_than>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator<=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::less_equal>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::greater_than>(lhs, Value<T>{rhs}));
}

template <Operator Op, class O1, class O2, class T>
constexpr auto operator>=(const Operation<Op, O1, O2>& lhs, const T& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::greater_equal>(lhs, Value<T>{rhs}));
}

// Logical operators for combining conditions
template <class L, Operator Op1, class R, class L2, Operator Op2, class R2>
constexpr auto operator&&(const Condition<L, Op1, R>& lhs, const Condition<L2, Op2, R2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_and>(lhs, rhs));
}

template <class L, Operator Op1, class R, class L2, Operator Op2, class R2>
constexpr auto operator||(const Condition<L, Op1, R>& lhs, const Condition<L2, Op2, R2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_or>(lhs, rhs));
}

// Logical operators for ConditionWrapper types
template <class T1, class T2>
constexpr auto operator&&(const ConditionWrapper<T1>& lhs, const ConditionWrapper<T2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_and>(lhs.condition, rhs.condition));
}

template <class T1, class T2>
constexpr auto operator||(const ConditionWrapper<T1>& lhs, const ConditionWrapper<T2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_or>(lhs.condition, rhs.condition));
}

// Mix ConditionWrapper with plain Condition
template <class T1, class L, Operator Op, class R>
constexpr auto operator&&(const ConditionWrapper<T1>& lhs, const Condition<L, Op, R>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_and>(lhs.condition, rhs));
}

template <class T1, class L, Operator Op, class R>
constexpr auto operator||(const ConditionWrapper<T1>& lhs, const Condition<L, Op, R>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_or>(lhs.condition, rhs));
}

template <class L, Operator Op, class R, class T2>
constexpr auto operator&&(const Condition<L, Op, R>& lhs, const ConditionWrapper<T2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_and>(lhs, rhs.condition));
}

template <class L, Operator Op, class R, class T2>
constexpr auto operator||(const Condition<L, Op, R>& lhs, const ConditionWrapper<T2>& rhs) {
    return make_condition_wrapper(
        make_condition<Operator::logical_or>(lhs, rhs.condition));
}

// ============================================================================
// JOIN
// ============================================================================

/// JOIN types
enum class JoinType {
    inner,
    left,
    right,
    full,
    cross
};

/// Convert JoinType to SQL keyword
constexpr std::string_view join_type_to_sql(JoinType type) {
    switch (type) {
        case JoinType::inner: return "INNER JOIN";
        case JoinType::left: return "LEFT OUTER JOIN";
        case JoinType::right: return "RIGHT OUTER JOIN";
        case JoinType::full: return "FULL OUTER JOIN";
        case JoinType::cross: return "CROSS JOIN";
    }
    return "";
}

/// Single JOIN clause representation
/// TableType: The table being joined
/// Alias: Optional table alias
/// ConditionType: The ON condition (Nothing for CROSS JOIN)
template <JoinType Type, class TableType, glz::string_literal Alias = "", class ConditionType = void>
struct Join {
    static constexpr JoinType join_type = Type;
    static constexpr std::string_view alias = Alias.sv();

    ConditionType condition;

    constexpr Join() = default;
    constexpr explicit Join(ConditionType cond) : condition(cond) {}

    constexpr bool has_alias() const noexcept {
        return !alias.empty();
    }

    constexpr std::string_view get_alias() const noexcept {
        return alias;
    }
};

/// List of JOINs (supports multiple JOINs in a single query)
template <class... Joins>
struct JoinList {
    std::tuple<Joins...> joins;

    constexpr JoinList() = default;
    constexpr explicit JoinList(Joins... js) : joins(std::make_tuple(js...)) {}
};

// ============================================================================
// TYPE CONVERSION
// ============================================================================

/// Trait to convert user-facing types to transpilation types
template <class T>
struct ToTranspilationType {
    using Type = Value<T>;

    constexpr Type operator()(const T& val) const noexcept {
        return Value<T>{val};
    }
};

/// Specialize for const char* to convert to std::string
template <>
struct ToTranspilationType<const char*> {
    using Type = Value<std::string>;

    Type operator()(const char* val) const noexcept {
        return Value<std::string>{std::string(val)};
    }
};

/// Specialize for char arrays to convert to std::string
template <int Length>
struct ToTranspilationType<char[Length]> {
    using Type = Value<std::string>;

    Type operator()(const char* val) const noexcept {
        return Value<std::string>{std::string(val)};
    }
};

/// Specialize for Col types - they're already transpilation types
template <glz::string_literal Name, glz::string_literal Alias>
struct ToTranspilationType<Col<Name, Alias>> {
    using Type = Col<Name, Alias>;

    constexpr Type operator()(const Col<Name, Alias>& col) const noexcept {
        return col;
    }
};

/// Helper function to convert a value to its transpilation type
template <class T>
constexpr auto to_transpilation_type(const T& t) {
    return ToTranspilationType<std::remove_cvref_t<T>>{}(t);
}

/// Overload for const char* to ensure proper conversion
inline auto to_transpilation_type(const char* t) {
    return ToTranspilationType<const char*>{}(t);
}

} // namespace sqlgen::transpilation

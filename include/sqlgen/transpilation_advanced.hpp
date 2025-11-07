#pragma once

#include "transpilation_core.hpp"
#include <glaze/reflection/to_tuple.hpp>
#include <glaze/reflection/get_name.hpp>
#include <glaze/core/meta.hpp>
#include "constraints/wrapper_types.hpp"
#include "constraints/traits.hpp"

namespace sqlgen::transpilation {

// ============================================================================
// AGGREGATE
// ============================================================================

/// Aggregate function types
enum class AggregateType {
    count,
    count_distinct,
    sum,
    avg,
    min,
    max
};

/// Convert AggregateType to SQL function name
constexpr std::string_view aggregate_type_to_sql(AggregateType type) {
    switch (type) {
        case AggregateType::count: return "COUNT";
        case AggregateType::count_distinct: return "COUNT";
        case AggregateType::sum: return "SUM";
        case AggregateType::avg: return "AVG";
        case AggregateType::min: return "MIN";
        case AggregateType::max: return "MAX";
    }
    return "";
}

/// Marker type for COUNT(*)
struct CountStar {};

/// Aggregate function representation
/// ExprType: The expression being aggregated (column, operation, or CountStar)
template <AggregateType Type, class ExprType>
struct Aggregate {
    static constexpr AggregateType aggregate_type = Type;

    ExprType expression;

    constexpr Aggregate() = default;
    constexpr explicit Aggregate(ExprType expr) : expression(expr) {}

    constexpr bool is_count_star() const noexcept {
        return std::is_same_v<ExprType, CountStar>;
    }

    constexpr bool is_distinct() const noexcept {
        return Type == AggregateType::count_distinct;
    }

    // Return descending order marker for ORDER BY
    constexpr auto desc() const noexcept {
        return make_desc(*this);
    }
};

// Forward declaration for Condition
template <class Left, Operator Op, class Right>
struct Condition;

// Comparison operators for Aggregate types
template <AggregateType Type, class ExprType, class T>
constexpr auto operator==(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator!=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator<(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator<=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator>(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <AggregateType Type, class ExprType, class T>
constexpr auto operator>=(const Aggregate<Type, ExprType>& lhs, const T& rhs) {
    return Condition<Aggregate<Type, ExprType>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

// ============================================================================
// FUNCTION
// ============================================================================

/// SQL function types
enum class FunctionType {
    // String functions
    concat,
    length,
    lower,
    upper,
    trim,
    ltrim,
    rtrim,
    replace,
    substring,

    // Math functions
    abs,
    ceil,
    floor,
    round,
    sqrt,
    exp,
    ln,
    log2,
    log10,
    pow,

    // Trigonometric functions
    sin,
    cos,
    tan,
    asin,
    acos,
    atan,

    // Date/time functions
    year,
    month,
    day,
    hour,
    minute,
    second,
    weekday,
    days_between,
    unixepoch,

    // Utility functions
    cast,
    coalesce
};

/// Convert function type to SQL function name
constexpr std::string_view function_type_to_sql(FunctionType type) {
    switch (type) {
        // String functions
        case FunctionType::concat: return "CONCAT";
        case FunctionType::length: return "LENGTH";
        case FunctionType::lower: return "LOWER";
        case FunctionType::upper: return "UPPER";
        case FunctionType::trim: return "TRIM";
        case FunctionType::ltrim: return "LTRIM";
        case FunctionType::rtrim: return "RTRIM";
        case FunctionType::replace: return "REPLACE";
        case FunctionType::substring: return "SUBSTR";

        // Math functions
        case FunctionType::abs: return "ABS";
        case FunctionType::ceil: return "CEIL";
        case FunctionType::floor: return "FLOOR";
        case FunctionType::round: return "ROUND";
        case FunctionType::sqrt: return "SQRT";
        case FunctionType::exp: return "EXP";
        case FunctionType::ln: return "LN";
        case FunctionType::log2: return "LOG2";
        case FunctionType::log10: return "LOG10";
        case FunctionType::pow: return "POW";

        // Trigonometric functions
        case FunctionType::sin: return "SIN";
        case FunctionType::cos: return "COS";
        case FunctionType::tan: return "TAN";
        case FunctionType::asin: return "ASIN";
        case FunctionType::acos: return "ACOS";
        case FunctionType::atan: return "ATAN";

        // Date/time functions - SQLite specific names
        case FunctionType::year: return "strftime";
        case FunctionType::month: return "strftime";
        case FunctionType::day: return "strftime";
        case FunctionType::hour: return "strftime";
        case FunctionType::minute: return "strftime";
        case FunctionType::second: return "strftime";
        case FunctionType::weekday: return "strftime";
        case FunctionType::days_between: return "julianday";
        case FunctionType::unixepoch: return "unixepoch";

        // Utility functions
        case FunctionType::cast: return "CAST";
        case FunctionType::coalesce: return "COALESCE";

        default: return "";
    }
}

/// SQL function with variadic arguments
template <FunctionType Type, class... ArgTypes>
struct Function {
    static constexpr FunctionType function_type = Type;
    std::tuple<ArgTypes...> arguments;

    constexpr Function(const ArgTypes&... args) : arguments(std::make_tuple(args...)) {}
};

/// Marker type for CAST target type
template <class T>
struct CastTarget {
    using type = T;
};

/// Specialized Function for CAST with target type
template <class TargetType, class ExprType>
struct CastFunction {
    static constexpr FunctionType function_type = FunctionType::cast;
    ExprType expression;

    constexpr CastFunction(const ExprType& expr) : expression(expr) {}
};

// Forward declarations for operations
enum class Operator;
template <Operator Op, class Operand1, class Operand2>
struct Operation;
template <class Left, Operator Op, class Right>
struct Condition;

// Arithmetic operators for Function types
template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator+(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::plus, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator-(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::minus, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator*(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::multiplies, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator/(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::divides, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator%(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Operation<Operator::mod, Function<Type, ArgTypes...>, T>{lhs, rhs};
}

// Comparison operators for Function types
template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator==(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator!=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator<(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator<=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator>(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <FunctionType Type, class... ArgTypes, class T>
constexpr auto operator>=(const Function<Type, ArgTypes...>& lhs, const T& rhs) {
    return Condition<Function<Type, ArgTypes...>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

// Comparison operators for CastFunction types
template <class TargetType, class ExprType, class T>
constexpr auto operator==(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator!=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::not_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator<(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::less_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator<=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::less_equal, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator>(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::greater_than, Value<T>>{lhs, Value<T>{rhs}};
}

template <class TargetType, class ExprType, class T>
constexpr auto operator>=(const CastFunction<TargetType, ExprType>& lhs, const T& rhs) {
    return Condition<CastFunction<TargetType, ExprType>, Operator::greater_equal, Value<T>>{lhs, Value<T>{rhs}};
}

// ============================================================================
// TABLE INFO
// ============================================================================

/// Get the table name for a type
/// Uses glz::meta<T>::name if available, otherwise uses type name
template <class T>
constexpr std::string_view get_table_name() {
    using Type = std::remove_cvref_t<T>;

    if constexpr (requires { glz::meta<Type>::name; }) {
        return glz::meta<Type>::name;
    } else {
        // Extract simple type name from __PRETTY_FUNCTION__ or similar
        std::string_view name = __PRETTY_FUNCTION__;

        #if defined(__GNUC__) || defined(__clang__)
        // Format: ... [T = TypeName]
        auto start = name.find("T = ");
        if (start != std::string_view::npos) {
            start += 4;
            auto end = name.find_first_of("];", start);
            name = name.substr(start, end - start);
        }
        #elif defined(_MSC_VER)
        // Format: ... <TypeName>
        auto start = name.find("get_table_name<");
        if (start != std::string_view::npos) {
            start += 15;
            auto end = name.find('>', start);
            name = name.substr(start, end - start);
        }
        #endif

        // Remove namespace qualifiers
        auto pos = name.rfind("::");
        if (pos != std::string_view::npos) {
            name = name.substr(pos + 2);
        }

        // Remove struct/class prefix
        if (name.starts_with("struct ")) {
            name.remove_prefix(7);
        } else if (name.starts_with("class ")) {
            name.remove_prefix(6);
        }

        return name;
    }
}

/// Field information for a struct field
struct FieldInfo {
    std::string name;
    std::string sql_type;
    bool nullable;
    constraints::FieldConstraints constraints;  // Constraint metadata
};

/// Get field information for a type using glaze reflection
template <class T>
std::vector<FieldInfo> get_fields() {
    using Type = std::remove_cvref_t<T>;
    std::vector<FieldInfo> fields;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            using FieldType = std::remove_cvref_t<decltype(
                glz::get<Is>(glz::to_tie(std::declval<Type&>())))>;

            FieldInfo info;
            info.name = std::string(glz::member_nameof<Is, Type>);
            info.nullable = is_optional_v<FieldType>;

            // Extract constraint metadata
            if constexpr (constraints::is_primary_key_v<FieldType>) {
                info.constraints.is_primary_key = true;
                info.constraints.auto_increment = constraints::is_auto_increment_v<FieldType>;
                info.constraints.is_not_null = true;  // Primary keys are always NOT NULL
                info.nullable = false;  // Override nullability
                // Get SQL type from underlying type
                using UnderlyingType = constraints::underlying_type_t<FieldType>;
                info.sql_type = std::string(to_sql_type<UnderlyingType>());
            }
            else if constexpr (constraints::is_unique_v<FieldType>) {
                info.constraints.is_unique = true;
                // Get SQL type from underlying type
                using UnderlyingType = constraints::underlying_type_t<FieldType>;
                info.sql_type = std::string(to_sql_type<UnderlyingType>());
            }
            else if constexpr (constraints::is_not_null_v<FieldType>) {
                info.constraints.is_not_null = true;
                info.nullable = false;  // Override nullability
                // Get SQL type from underlying type
                using UnderlyingType = constraints::underlying_type_t<FieldType>;
                info.sql_type = std::string(to_sql_type<UnderlyingType>());
            }
            else if constexpr (constraints::is_foreign_key_v<FieldType>) {
                // Extract foreign key metadata
                using FK = constraints::foreign_key_info<FieldType>;
                constraints::ForeignKeyReference fk_info;
                fk_info.table = std::string(transpilation::get_table_name<typename FK::referenced_table>());
                fk_info.column = std::string(FK::referenced_column.sv());
                fk_info.on_delete = std::string(referential_action_to_sql(FK::on_delete));
                fk_info.on_update = std::string(referential_action_to_sql(FK::on_update));
                info.constraints.foreign_key = fk_info;
                // Get SQL type from underlying type
                using UnderlyingType = constraints::underlying_type_t<FieldType>;
                info.sql_type = std::string(to_sql_type<UnderlyingType>());
            }
            else if constexpr (constraints::is_varchar_v<FieldType>) {
                // Varchar with length constraint
                constexpr size_t length = constraints::varchar_length_v<FieldType>;
                info.sql_type = "VARCHAR(" + std::to_string(length) + ")";
            }
            else if constexpr (constraints::is_char_v<FieldType>) {
                // Char with fixed length
                constexpr size_t length = constraints::char_length_v<FieldType>;
                info.sql_type = "CHAR(" + std::to_string(length) + ")";
            }
            else {
                // No constraint wrapper, use FieldType directly
                info.sql_type = std::string(to_sql_type<FieldType>());
            }

            fields.push_back(std::move(info));
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return fields;
}

/// Generate a comma-separated list of field names
template <class T>
std::string get_field_list() {
    auto fields = get_fields<T>();
    std::string result;

    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) result += ", ";
        result += quote_identifier(fields[i].name);
    }

    return result;
}

/// Generate CREATE TABLE statement for a type
template <class T>
std::string create_table_sql(bool if_not_exists = false) {
    std::string sql = "CREATE TABLE ";

    if (if_not_exists) {
        sql += "IF NOT EXISTS ";
    }

    sql += quote_identifier(std::string(get_table_name<T>()));
    sql += " (\n";

    auto fields = get_fields<T>();
    std::vector<std::string> table_constraints;  // For FOREIGN KEY constraints

    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) sql += ",\n";
        sql += "    ";
        sql += quote_identifier(fields[i].name);
        sql += " ";
        sql += fields[i].sql_type;

        // Add PRIMARY KEY constraint
        if (fields[i].constraints.is_primary_key) {
            sql += " PRIMARY KEY";
            if (fields[i].constraints.auto_increment) {
                sql += " AUTOINCREMENT";
            }
        }

        // Add UNIQUE constraint
        if (fields[i].constraints.is_unique) {
            sql += " UNIQUE";
        }

        // Add NOT NULL constraint (if not already added by PRIMARY KEY)
        if (!fields[i].nullable || fields[i].constraints.is_not_null) {
            if (!fields[i].constraints.is_primary_key) {  // PRIMARY KEY implies NOT NULL
                sql += " NOT NULL";
            }
        }

        // Collect FOREIGN KEY constraints for table-level addition
        if (fields[i].constraints.foreign_key) {
            auto& fk = *fields[i].constraints.foreign_key;
            std::string fk_constraint = "FOREIGN KEY (" +
                quote_identifier(fields[i].name) + ") REFERENCES " +
                quote_identifier(fk.table) + "(" + quote_identifier(fk.column) + ")";

            // Add ON DELETE/UPDATE actions if specified
            if (!fk.on_delete.empty()) {
                fk_constraint += " ON DELETE " + fk.on_delete;
            }
            if (!fk.on_update.empty()) {
                fk_constraint += " ON UPDATE " + fk.on_update;
            }

            table_constraints.push_back(fk_constraint);
        }
    }

    // Add table-level constraints (FOREIGN KEY)
    for (const auto& constraint : table_constraints) {
        sql += ",\n    " + constraint;
    }

    sql += "\n)";
    return sql;
}

// ============================================================================
// FIELD LIST
// ============================================================================

/// Generate a SELECT field list from a type
template <class T>
std::string select_field_list(std::string_view table_alias = "") {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";

            if (!table_alias.empty()) {
                result += quote_identifier(std::string(table_alias));
                result += ".";
            }

            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate a SELECT field list with explicit aliasing
template <class T>
std::string select_field_list_with_alias(std::string_view table_alias) {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";

            result += quote_identifier(std::string(table_alias));
            result += ".";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            result += " AS ";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));

            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate an INSERT field list (just field names)
template <class T>
std::string insert_field_list() {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate placeholder list for INSERT VALUES
template <class T>
std::string insert_placeholders() {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    for (size_t i = 0; i < field_count; ++i) {
        if (i > 0) result += ", ";
        result += "?";
    }

    return result;
}

} // namespace sqlgen::transpilation

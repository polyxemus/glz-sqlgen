#pragma once

#include <string>
#include <sstream>
#include <concepts>
#include "Col.hpp"
#include "Value.hpp"
#include "Operation.hpp"
#include "Condition.hpp"
#include "../advanced_conditions.hpp"
#include "Operator.hpp"
#include "Desc.hpp"
#include "Set.hpp"
#include "Aggregate.hpp"
#include "quote.hpp"

// Forward declaration for user-facing Col
namespace glz_sqlgen {
template <glz::string_literal Name, glz::string_literal Alias>
struct Col;
}

namespace glz_sqlgen::transpilation {

/// Convert an operator to SQL string
constexpr std::string_view operator_to_sql(Operator op) {
    switch (op) {
        case Operator::equal: return " = ";
        case Operator::not_equal: return " != ";
        case Operator::less_than: return " < ";
        case Operator::less_equal: return " <= ";
        case Operator::greater_than: return " > ";
        case Operator::greater_equal: return " >= ";
        case Operator::plus: return " + ";
        case Operator::minus: return " - ";
        case Operator::multiplies: return " * ";
        case Operator::divides: return " / ";
        case Operator::mod: return " % ";
        case Operator::logical_and: return " AND ";
        case Operator::logical_or: return " OR ";
        case Operator::like: return " LIKE ";
        case Operator::not_like: return " NOT LIKE ";
        case Operator::ilike: return " ILIKE ";
        case Operator::not_ilike: return " NOT ILIKE ";
        case Operator::in: return " IN ";
        case Operator::not_in: return " NOT IN ";
        case Operator::is_null: return " IS NULL";
        case Operator::is_not_null: return " IS NOT NULL";
        case Operator::between: return " BETWEEN ";
        case Operator::not_between: return " NOT BETWEEN ";
        default: return " ";
    }
}

/// Forward declarations for recursive calls
template <class T>
std::string to_sql(const T& value);

/// Convert a transpilation column to SQL
template <glz::string_literal Name, glz::string_literal Alias>
std::string to_sql(const Col<Name, Alias>& col) {
    if (col.has_alias()) {
        return quote_identifier(std::string(col.alias)) + "." +
               quote_identifier(std::string(col.name));
    }
    return quote_identifier(std::string(col.name));
}

} // namespace glz_sqlgen::transpilation

// Overload for user-facing Col - must be declared before Set to_sql
namespace glz_sqlgen::transpilation {
template <glz::string_literal Name, glz::string_literal Alias>
inline std::string to_sql(const glz_sqlgen::Col<Name, Alias>& col) {
    // Use conversion operator to get transpilation::Col
    typename glz_sqlgen::Col<Name, Alias>::ColType tcol = col;
    return to_sql(tcol);
}
} // namespace glz_sqlgen::transpilation

namespace glz_sqlgen::transpilation {

/// Forward declaration for Col and Operation type checking
template <glz::string_literal, glz::string_literal>
struct Col;

template <Operator, class, class>
struct Operation;

/// Overloads for primitive types (when not wrapped in Value)
inline std::string to_sql(int value) {
    return std::to_string(value);
}

inline std::string to_sql(int64_t value) {
    return std::to_string(value);
}

inline std::string to_sql(uint32_t value) {
    return std::to_string(value);
}

inline std::string to_sql(uint64_t value) {
    return std::to_string(value);
}

inline std::string to_sql(double value) {
    return std::to_string(value);
}

inline std::string to_sql(float value) {
    return std::to_string(value);
}

inline std::string to_sql(bool value) {
    return value ? "1" : "0";
}

inline std::string to_sql(const std::string& value) {
    return quote_string(value);
}

inline std::string to_sql(const char* value) {
    return quote_string(std::string(value));
}

/// Convert a value to SQL
template <class T>
std::string to_sql(const Value<T>& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return quote_string(value.get());
    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                         std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
        return std::to_string(value.get());
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
        return std::to_string(value.get());
    } else if constexpr (std::is_same_v<T, bool>) {
        return value.get() ? "1" : "0";
    } else {
        // For Col, Operation, and other transpilation types, recursively call to_sql
        return to_sql(value.get());
    }
}

/// Convert an operation to SQL
template <Operator Op, class Operand1, class Operand2>
std::string to_sql(const Operation<Op, Operand1, Operand2>& operation) {
    std::string result = "(";
    result += to_sql(operation.operand1);
    result += operator_to_sql(Op);
    result += to_sql(operation.operand2);
    result += ")";
    return result;
}

} // namespace glz_sqlgen::transpilation

// Forward declarations for advanced condition types
namespace glz_sqlgen::advanced {
    template <class ColType> struct IsNullCondition;
    template <class ColType> struct IsNotNullCondition;
    template <class ColType, class... ValueTypes> struct InCondition;
    template <class ColType, class... ValueTypes> struct NotInCondition;
    template <class ColType, class LowerType, class UpperType> struct BetweenCondition;
    template <class ColType, class LowerType, class UpperType> struct NotBetweenCondition;
}

namespace glz_sqlgen::transpilation {

// Forward declarations for to_sql of advanced conditions
template <class ColType>
inline std::string to_sql(const glz_sqlgen::advanced::IsNullCondition<ColType>& cond);

template <class ColType>
inline std::string to_sql(const glz_sqlgen::advanced::IsNotNullCondition<ColType>& cond);

template <class ColType, class... ValueTypes>
inline std::string to_sql(const glz_sqlgen::advanced::InCondition<ColType, ValueTypes...>& cond);

template <class ColType, class... ValueTypes>
inline std::string to_sql(const glz_sqlgen::advanced::NotInCondition<ColType, ValueTypes...>& cond);

template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const glz_sqlgen::advanced::BetweenCondition<ColType, LowerType, UpperType>& cond);

template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const glz_sqlgen::advanced::NotBetweenCondition<ColType, LowerType, UpperType>& cond);


template <class ColType>
inline std::string to_sql(const glz_sqlgen::advanced::IsNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NULL";
}

/// Convert IS NOT NULL condition to SQL
template <class ColType>
inline std::string to_sql(const glz_sqlgen::advanced::IsNotNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NOT NULL";
}

/// Convert IN condition to SQL
template <class ColType, class... ValueTypes>
inline std::string to_sql(const glz_sqlgen::advanced::InCondition<ColType, ValueTypes...>& cond) {
    std::string sql = to_sql(cond.column) + " IN (";
    bool first = true;
    std::apply([&](const auto&... values) {
        (([&](const auto& val) {
            if (!first) sql += ", ";
            sql += to_sql(Value{val});
            first = false;
        }(values)), ...);
    }, cond.values);
    sql += ")";
    return sql;
}

/// Convert NOT IN condition to SQL
template <class ColType, class... ValueTypes>
inline std::string to_sql(const glz_sqlgen::advanced::NotInCondition<ColType, ValueTypes...>& cond) {
    std::string sql = to_sql(cond.column) + " NOT IN (";
    bool first = true;
    std::apply([&](const auto&... values) {
        (([&](const auto& val) {
            if (!first) sql += ", ";
            sql += to_sql(Value{val});
            first = false;
        }(values)), ...);
    }, cond.values);
    sql += ")";
    return sql;
}

/// Convert BETWEEN condition to SQL
template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const glz_sqlgen::advanced::BetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " BETWEEN " +
           to_sql(Value{cond.lower}) + " AND " +
           to_sql(Value{cond.upper});
}

/// Convert NOT BETWEEN condition to SQL
template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const glz_sqlgen::advanced::NotBetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " NOT BETWEEN " +
           to_sql(Value{cond.lower}) + " AND " +
           to_sql(Value{cond.upper});
}
/// Helper to check if a Condition contains a logical operator
template <class T>
constexpr bool is_logical_condition = false;

// Specialize for Condition types with logical operators
template <class L, class R>
constexpr bool is_logical_condition<Condition<L, Operator::logical_and, R>> = true;

template <class L, class R>
constexpr bool is_logical_condition<Condition<L, Operator::logical_or, R>> = true;

/// Convert a condition to SQL
template <class Left, Operator Op, class Right>
std::string to_sql(const Condition<Left, Op, Right>& condition) {
    std::string result;

    // Add parentheses around operands only if:
    // 1. This is a logical operator (AND/OR), AND
    // 2. The operand itself contains a logical operator
    constexpr bool is_logical = (Op == Operator::logical_and || Op == Operator::logical_or);

    if constexpr (is_logical && is_logical_condition<std::decay_t<Left>>) {
        result = "(" + to_sql(condition.left) + ")";
    } else {
        result = to_sql(condition.left);
    }

    result += operator_to_sql(Op);

    if constexpr (is_logical && is_logical_condition<std::decay_t<Right>>) {
        result += "(" + to_sql(condition.right) + ")";
    } else {
        result += to_sql(condition.right);
    }

    return result;
}

/// Convert a condition wrapper to SQL
template <class T>
std::string to_sql(const ConditionWrapper<T>& wrapper) {
    return to_sql(wrapper.condition);
}

/// Convert a Desc (descending order column) to SQL
template <class Col>
std::string to_sql(const Desc<Col>& desc) {
    return to_sql(desc.column) + " DESC";
}

/// Convert a Set (UPDATE SET clause) to SQL
template <class Col, class Value>
std::string to_sql(const Set<Col, Value>& set) {
    return to_sql(set.column) + " = " + to_sql(set.value);
}

/// Convert an Aggregate function to SQL
template <AggregateType Type, class ExprType>
std::string to_sql(const Aggregate<Type, ExprType>& agg) {
    std::string sql;
    sql += aggregate_type_to_sql(Type);
    sql += "(";

    if constexpr (std::is_same_v<ExprType, CountStar>) {
        // COUNT(*)
        sql += "*";
    } else {
        // Add DISTINCT for COUNT(DISTINCT col)
        if (agg.is_distinct()) {
            sql += "DISTINCT ";
        }
        // Add expression
        sql += to_sql(agg.expression);
    }

    sql += ")";
    return sql;
}

} // namespace glz_sqlgen::transpilation


/// Convert IS NULL condition to SQL


// SQL Functions - Include at the end
#include "Function.hpp"

namespace glz_sqlgen::transpilation {

/// Helper to get SQL type name for CAST
template <class T>
constexpr std::string_view get_sql_type_name() {
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        return "INTEGER";
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
        return "REAL";
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "TEXT";
    } else if constexpr (std::is_same_v<T, bool>) {
        return "INTEGER";
    } else {
        return "TEXT";
    }
}

/// Convert a SQL function to SQL string
template <FunctionType Type, class... ArgTypes>
std::string to_sql(const Function<Type, ArgTypes...>& func) {
    std::string sql;
    
    // Handle date/time functions specially (SQLite strftime format)
    if constexpr (Type == FunctionType::year) {
        sql = "CAST(strftime('%Y', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::month) {
        sql = "CAST(strftime('%m', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::day) {
        sql = "CAST(strftime('%d', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::hour) {
        sql = "CAST(strftime('%H', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::minute) {
        sql = "CAST(strftime('%M', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::second) {
        sql = "CAST(strftime('%S', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::weekday) {
        sql = "CAST(strftime('%w', ";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ") AS INTEGER)";
        return sql;
    } else if constexpr (Type == FunctionType::days_between) {
        sql = "(julianday(";
        // Get first argument
        sql += to_sql(std::get<1>(func.arguments));
        sql += ") - julianday(";
        sql += to_sql(std::get<0>(func.arguments));
        sql += "))";
        return sql;
    } else if constexpr (Type == FunctionType::unixepoch) {
        sql = "unixepoch(";
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        sql += ")";
        return sql;
    } else {
        // Standard function call: FUNC_NAME(arg1, arg2, ...)
        sql += function_type_to_sql(Type);
        sql += "(";
        
        std::apply([&](const auto&... args) {
            bool first = true;
            (([&] {
                if (!first) sql += ", ";
                sql += to_sql(args);
                first = false;
            }()), ...);
        }, func.arguments);
        
        sql += ")";
    }
    
    return sql;
}

/// Convert a CAST function to SQL
template <class TargetType, class ExprType>
std::string to_sql(const CastFunction<TargetType, ExprType>& func) {
    std::string sql = "CAST(";
    sql += to_sql(func.expression);
    sql += " AS ";
    sql += get_sql_type_name<TargetType>();
    sql += ")";
    return sql;
}

} // namespace glz_sqlgen::transpilation

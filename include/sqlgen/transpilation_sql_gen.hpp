#pragma once

#include "transpilation_advanced.hpp"
#include "advanced_conditions.hpp"

// Forward declaration for user-facing Col
namespace sqlgen {
template <glz::string_literal Name, glz::string_literal Alias>
struct Col;
}

namespace sqlgen::transpilation {

// ============================================================================
// CORE to_sql - from to_sql_string.hpp
// ============================================================================

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

// Overload for user-facing Col - must be declared before Set to_sql
template <glz::string_literal Name, glz::string_literal Alias>
inline std::string to_sql(const sqlgen::Col<Name, Alias>& col) {
    // Use conversion operator to get transpilation::Col
    typename sqlgen::Col<Name, Alias>::ColType tcol = col;
    return to_sql(tcol);
}

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
    std::string str = std::to_string(value);
    // Remove trailing zeros after decimal point
    if (str.find('.') != std::string::npos) {
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        // Remove trailing decimal point if no fractional part
        if (str.back() == '.') {
            str.pop_back();
        }
    }
    return str;
}

inline std::string to_sql(float value) {
    return to_sql(static_cast<double>(value));
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
                         std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
                         std::is_same_v<T, double> || std::is_same_v<T, float> ||
                         std::is_same_v<T, bool>) {
        // Use the standalone to_sql functions for proper formatting
        return to_sql(value.get());
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

} // namespace sqlgen::transpilation

// Forward declarations for advanced condition types
namespace sqlgen::advanced {
    template <class ColType> struct IsNullCondition;
    template <class ColType> struct IsNotNullCondition;
    template <class ColType, class... ValueTypes> struct InCondition;
    template <class ColType, class... ValueTypes> struct NotInCondition;
    template <class ColType, class LowerType, class UpperType> struct BetweenCondition;
    template <class ColType, class LowerType, class UpperType> struct NotBetweenCondition;
}

namespace sqlgen::transpilation {

// Forward declarations for to_sql of advanced conditions
template <class ColType>
inline std::string to_sql(const ::sqlgen::advanced::IsNullCondition<ColType>& cond);

template <class ColType>
inline std::string to_sql(const ::sqlgen::advanced::IsNotNullCondition<ColType>& cond);

template <class ColType, class... ValueTypes>
inline std::string to_sql(const ::sqlgen::advanced::InCondition<ColType, ValueTypes...>& cond);

template <class ColType, class... ValueTypes>
inline std::string to_sql(const ::sqlgen::advanced::NotInCondition<ColType, ValueTypes...>& cond);

template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const ::sqlgen::advanced::BetweenCondition<ColType, LowerType, UpperType>& cond);

template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const ::sqlgen::advanced::NotBetweenCondition<ColType, LowerType, UpperType>& cond);


template <class ColType>
inline std::string to_sql(const ::sqlgen::advanced::IsNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NULL";
}

/// Convert IS NOT NULL condition to SQL
template <class ColType>
inline std::string to_sql(const ::sqlgen::advanced::IsNotNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NOT NULL";
}

/// Convert IN condition to SQL
template <class ColType, class... ValueTypes>
inline std::string to_sql(const ::sqlgen::advanced::InCondition<ColType, ValueTypes...>& cond) {
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
inline std::string to_sql(const ::sqlgen::advanced::NotInCondition<ColType, ValueTypes...>& cond) {
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
inline std::string to_sql(const ::sqlgen::advanced::BetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " BETWEEN " +
           to_sql(Value{cond.lower}) + " AND " +
           to_sql(Value{cond.upper});
}

/// Convert NOT BETWEEN condition to SQL
template <class ColType, class LowerType, class UpperType>
inline std::string to_sql(const ::sqlgen::advanced::NotBetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " NOT BETWEEN " +
           to_sql(Value{cond.lower}) + " AND " +
           to_sql(Value{cond.upper});
}
/// Helper to extract operator from a Condition type
template <class T>
struct GetOperator {
    static constexpr Operator value = Operator::equal; // Default, won't be used
};

template <class L, Operator Op, class R>
struct GetOperator<Condition<L, Op, R>> {
    static constexpr Operator value = Op;
};

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
    // 2. The operand contains a DIFFERENT logical operator (to preserve precedence)
    constexpr bool is_logical = (Op == Operator::logical_and || Op == Operator::logical_or);

    if constexpr (is_logical && is_logical_condition<std::decay_t<Left>>) {
        constexpr Operator left_op = GetOperator<std::decay_t<Left>>::value;
        // Only add parentheses if operators differ (mixing AND and OR)
        if constexpr (left_op != Op) {
            result = "(" + to_sql(condition.left) + ")";
        } else {
            result = to_sql(condition.left);
        }
    } else {
        result = to_sql(condition.left);
    }

    result += operator_to_sql(Op);

    if constexpr (is_logical && is_logical_condition<std::decay_t<Right>>) {
        // Always add parentheses to right operands that are logical conditions
        // This preserves grouping and improves readability
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


// ============================================================================
// WHERE CLAUSE
// ============================================================================



/// Generate a WHERE clause from a condition
template <class CondType>
std::string where_clause(const CondType& condition) {
    return "WHERE " + to_sql(condition);
}

/// Generate a WHERE clause from a condition wrapper
template <class T>
std::string where_clause(const ConditionWrapper<T>& wrapper) {
    return "WHERE " + to_sql(wrapper.condition);
}

/// Combine multiple conditions with AND
template <class... Conditions>
std::string where_clause_and(const Conditions&... conditions) {
    std::string result = "WHERE ";
    size_t idx = 0;

    ([&] {
        if (idx > 0) result += " AND ";
        result += to_sql(conditions);
        ++idx;
    }(), ...);

    return result;
}

/// Combine multiple conditions with OR
template <class... Conditions>
std::string where_clause_or(const Conditions&... conditions) {
    std::string result = "WHERE ";
    size_t idx = 0;

    ([&] {
        if (idx > 0) result += " OR ";
        result += to_sql(conditions);
        ++idx;
    }(), ...);

    return result;
}


// ============================================================================
// GROUP BY CLAUSE
// ============================================================================



/// Generate SQL for GROUP BY clause
template <class... ColTypes>
std::string group_by_sql(const std::tuple<ColTypes...>& columns) {
    std::string sql = "GROUP BY ";

    bool first = true;
    std::apply([&](const auto&... cols) {
        (([&] {
            if (!first) {
                sql += ", ";
            }
            sql += to_sql(cols);
            first = false;
        }()), ...);
    }, columns);

    return sql;
}


// ============================================================================
// HAVING CLAUSE
// ============================================================================



/// Generate SQL for HAVING clause
template <class ConditionType>
std::string having_clause(const ConditionType& condition) {
    return "HAVING " + to_sql(condition);
}


// ============================================================================
// ORDER BY & LIMIT
// ============================================================================



/// Generate ORDER BY SQL from tuple of columns
template <class... ColTypes>
std::string order_by_sql(const std::tuple<ColTypes...>& columns) {
    std::string result = "ORDER BY ";
    bool first = true;

    std::apply([&](const auto&... cols) {
        (([&] {
            if (first) {
                result += to_sql(cols);
                first = false;
            } else {
                result += ", " + to_sql(cols);
            }
        }()), ...);
    }, columns);

    return result;
}

/// Generate LIMIT SQL
inline std::string limit_sql(size_t limit_value, const std::optional<size_t>& offset_value) {
    std::string result = "LIMIT " + std::to_string(limit_value);
    if (offset_value.has_value()) {
        result += " OFFSET " + std::to_string(offset_value.value());
    }
    return result;
}


// ============================================================================
// JOIN CLAUSE
// ============================================================================



/// Generate SQL for a single JOIN clause
template <JoinType Type, class TableType, glz::string_literal Alias, class ConditionType>
std::string join_sql(const Join<Type, TableType, Alias, ConditionType>& join) {
    std::string sql;

    // Add JOIN type
    sql += join_type_to_sql(Type);
    sql += " ";

    // Add table name
    sql += quote_identifier(get_table_name<TableType>());

    // Add alias if present
    if (join.has_alias()) {
        sql += " AS ";
        sql += quote_identifier(join.get_alias());
    }

    // Add ON condition (except for CROSS JOIN)
    if constexpr (Type != JoinType::cross) {
        sql += " ON ";
        sql += to_sql(join.condition);
    }

    return sql;
}

/// Generate SQL for a list of JOINs
template <class... Joins>
std::string joins_sql(const JoinList<Joins...>& join_list) {
    std::string sql;
    bool first = true;

    std::apply([&](const auto&... joins) {
        (([&] {
            if (!first) {
                sql += " ";
            }
            sql += join_sql(joins);
            first = false;
        }()), ...);
    }, join_list.joins);

    return sql;
}


// ============================================================================
// AGGREGATE SQL
// ============================================================================



/// Generate SQL for aggregate function
template <AggregateType Type, class ExprType>
std::string aggregate_sql(const Aggregate<Type, ExprType>& agg) {
    std::string sql;

    // Add function name
    sql += aggregate_type_to_sql(Type);
    sql += "(";

    // Handle different cases
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

} // namespace sqlgen::transpilation

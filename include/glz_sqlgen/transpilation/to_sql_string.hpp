#pragma once

#include <string>
#include <sstream>
#include <concepts>
#include "Col.hpp"
#include "Value.hpp"
#include "Operation.hpp"
#include "Condition.hpp"
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
        // For other types, try to convert to string
        std::ostringstream oss;
        oss << value.get();
        return quote_string(oss.str());
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

/// Convert a condition to SQL
template <class Left, Operator Op, class Right>
std::string to_sql(const Condition<Left, Op, Right>& condition) {
    std::string result = to_sql(condition.left);
    result += operator_to_sql(Op);
    result += to_sql(condition.right);
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

// Include advanced conditions at the end
#include "../advanced_conditions.hpp"

/// Convert IS NULL condition to SQL
template <class ColType>
std::string to_sql(const glz_sqlgen::advanced::IsNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NULL";
}

/// Convert IS NOT NULL condition to SQL
template <class ColType>
std::string to_sql(const glz_sqlgen::advanced::IsNotNullCondition<ColType>& cond) {
    return to_sql(cond.column) + " IS NOT NULL";
}

/// Convert IN condition to SQL
template <class ColType, class... ValueTypes>
std::string to_sql(const glz_sqlgen::advanced::InCondition<ColType, ValueTypes...>& cond) {
    std::string sql = to_sql(cond.column) + " IN (";
    bool first = true;
    std::apply([&](const auto&... values) {
        (([&] {
            if (!first) sql += ", ";
            sql += to_sql(transpilation::Value{values});
            first = false;
        }()), ...);
    }, cond.values);
    sql += ")";
    return sql;
}

/// Convert NOT IN condition to SQL
template <class ColType, class... ValueTypes>
std::string to_sql(const glz_sqlgen::advanced::NotInCondition<ColType, ValueTypes...>& cond) {
    std::string sql = to_sql(cond.column) + " NOT IN (";
    bool first = true;
    std::apply([&](const auto&... values) {
        (([&] {
            if (!first) sql += ", ";
            sql += to_sql(transpilation::Value{values});
            first = false;
        }()), ...);
    }, cond.values);
    sql += ")";
    return sql;
}

/// Convert BETWEEN condition to SQL
template <class ColType, class LowerType, class UpperType>
std::string to_sql(const glz_sqlgen::advanced::BetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " BETWEEN " +
           to_sql(transpilation::Value{cond.lower}) + " AND " +
           to_sql(transpilation::Value{cond.upper});
}

/// Convert NOT BETWEEN condition to SQL
template <class ColType, class LowerType, class UpperType>
std::string to_sql(const glz_sqlgen::advanced::NotBetweenCondition<ColType, LowerType, UpperType>& cond) {
    return to_sql(cond.column) + " NOT BETWEEN " +
           to_sql(transpilation::Value{cond.lower}) + " AND " +
           to_sql(transpilation::Value{cond.upper});
}


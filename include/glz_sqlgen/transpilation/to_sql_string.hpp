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
#include "quote.hpp"

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
        default: return " ";
    }
}

/// Forward declarations for recursive calls
template <class T>
std::string to_sql(const T& value);

} // namespace glz_sqlgen::transpilation

// Forward declare user-facing Col type
#ifndef GLZ_SQLGEN_COL_HPP_INCLUDED
namespace glz_sqlgen {
template <glz::string_literal Name, glz::string_literal Alias>
struct Col;
}
#endif

namespace glz_sqlgen::transpilation {

/// Convert a transpilation column to SQL
template <glz::string_literal Name, glz::string_literal Alias>
std::string to_sql(const Col<Name, Alias>& col) {
    if (col.has_alias()) {
        return quote_identifier(std::string(col.alias)) + "." +
               quote_identifier(std::string(col.name));
    }
    return quote_identifier(std::string(col.name));
}

/// Convert user-facing Col to SQL (delegates to transpilation Col)
template <glz::string_literal Name, glz::string_literal Alias>
inline std::string to_sql(const glz_sqlgen::Col<Name, Alias>& /*col*/) {
    // Delegate to transpilation Col
    return to_sql(Col<Name, Alias>{});
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

} // namespace glz_sqlgen::transpilation

// Overload for user-facing Col type
// Forward declare if not yet included
#ifndef GLZ_SQLGEN_COL_HPP_INCLUDED
namespace glz_sqlgen {
template <glz::string_literal Name, glz::string_literal Alias>
struct Col;
}
#endif

namespace glz_sqlgen::transpilation {
/// Convert user-facing Col to SQL (delegates to transpilation Col)
template <glz::string_literal Name, glz::string_literal Alias>
inline std::string to_sql(const glz_sqlgen::Col<Name, Alias>& col) {
    // Delegate to transpilation Col
    return to_sql(Col<Name, Alias>{});
}
} // namespace glz_sqlgen::transpilation

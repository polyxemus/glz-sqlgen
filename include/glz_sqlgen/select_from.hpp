#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include "common_types.hpp"
#include "where.hpp"
#include "order_by.hpp"
#include "limit.hpp"
#include "transpilation/field_list.hpp"
#include "transpilation/table_info.hpp"
#include "transpilation/where_clause.hpp"
#include "transpilation/order_by_limit.hpp"

namespace glz_sqlgen {

/// SELECT query builder
template <class TableType, class FieldsTuple = Nothing, class WhereType = Nothing,
          class OrderByType = Nothing, class LimitType = Nothing>
struct SelectFrom {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "SELECT ";

        // Add fields
        if constexpr (std::is_same_v<FieldsTuple, Nothing>) {
            // SELECT *
            sql += transpilation::select_field_list<TableType>();
        } else {
            // SELECT specified fields
            bool first = true;
            std::apply([&](const auto&... fields) {
                (([&] {
                    if (first) {
                        sql += transpilation::to_sql(fields);
                        first = false;
                    } else {
                        sql += ", " + transpilation::to_sql(fields);
                    }
                }()), ...);
            }, fields_);
        }

        // Add FROM
        sql += " FROM ";
        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        // Add ORDER BY if specified
        if constexpr (!std::is_same_v<OrderByType, Nothing>) {
            sql += " ";
            sql += transpilation::order_by_sql(order_by_.columns);
        }

        // Add LIMIT if specified
        if constexpr (!std::is_same_v<LimitType, Nothing>) {
            sql += " ";
            sql += transpilation::limit_sql(limit_.limit_value, limit_.offset_value);
        }

        return sql;
    }

    /// Pipe operator for WHERE clause
    template <class ConditionType>
    friend auto operator|(const SelectFrom& s, const Where<ConditionType>& w) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call where() twice");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call order_by() before where()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() before where()");

        return SelectFrom<TableType, FieldsTuple, ConditionType, OrderByType, LimitType>{
            .fields_ = s.fields_,
            .where_ = w.condition,
            .order_by_ = s.order_by_,
            .limit_ = s.limit_
        };
    }

    /// Pipe operator for ORDER BY clause
    template <class... ColTypes>
    friend auto operator|(const SelectFrom& s, const OrderBy<ColTypes...>& o) {
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call order_by() twice");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() before order_by()");

        return SelectFrom<TableType, FieldsTuple, WhereType, OrderBy<ColTypes...>, LimitType>{
            .fields_ = s.fields_,
            .where_ = s.where_,
            .order_by_ = o,
            .limit_ = s.limit_
        };
    }

    /// Pipe operator for LIMIT clause
    friend auto operator|(const SelectFrom& s, const Limit& l) {
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() twice");

        return SelectFrom<TableType, FieldsTuple, WhereType, OrderByType, Limit>{
            .fields_ = s.fields_,
            .where_ = s.where_,
            .order_by_ = s.order_by_,
            .limit_ = l
        };
    }

    FieldsTuple fields_;
    WhereType where_;
    OrderByType order_by_;
    LimitType limit_;
};

/// Create a SELECT * FROM Table query
template <class TableType>
auto select_from() {
    return SelectFrom<TableType>{};
}

/// Create a SELECT fields... FROM Table query
template <class TableType, class... FieldTypes>
auto select_from(const FieldTypes&... fields) {
    using FieldsTuple = std::tuple<FieldTypes...>;
    return SelectFrom<TableType, FieldsTuple>{
        .fields_ = std::make_tuple(fields...)
    };
}

} // namespace glz_sqlgen

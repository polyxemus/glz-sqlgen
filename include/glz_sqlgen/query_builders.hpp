#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <ranges>
#include "core.hpp"
#include "query_clauses.hpp"
#include "transpilation/field_list.hpp"
#include "transpilation/table_info.hpp"
#include "transpilation/where_clause.hpp"
#include "transpilation/order_by_limit.hpp"
#include "transpilation/join_clause.hpp"
#include "transpilation/group_by_clause.hpp"
#include "transpilation/having_clause.hpp"
#include "transpilation/to_sql_string.hpp"
#include "transpilation/Set.hpp"
#include "transpilation/quote.hpp"

namespace glz_sqlgen {

// ============================================================================
// SELECT Query Builder
// ============================================================================

/// SELECT query builder
template <class TableType, class FieldsTuple = Nothing, class JoinListType = Nothing,
          class WhereType = Nothing, class GroupByType = Nothing, class HavingType = Nothing,
          class OrderByType = Nothing, class LimitType = Nothing>
struct SelectFrom {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "SELECT ";

        // Add fields
        if constexpr (std::is_same_v<FieldsTuple, Nothing>) {
            // SELECT * - use table prefix if there are JOINs
            if constexpr (!std::is_same_v<JoinListType, Nothing>) {
                sql += transpilation::select_field_list<TableType>(transpilation::get_table_name<TableType>());
            } else {
                sql += transpilation::select_field_list<TableType>();
            }
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

        // Add JOINs if specified
        if constexpr (!std::is_same_v<JoinListType, Nothing>) {
            sql += " ";
            sql += transpilation::joins_sql(joins_);
        }

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        // Add GROUP BY if specified
        if constexpr (!std::is_same_v<GroupByType, Nothing>) {
            sql += " ";
            sql += transpilation::group_by_sql(group_by_.columns);
        }

        // Add HAVING if specified
        if constexpr (!std::is_same_v<HavingType, Nothing>) {
            sql += " ";
            sql += transpilation::having_clause(having_);
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

    /// Pipe operator for JOINs - single JOIN
    template <class JoinTableType, glz::string_literal JoinAlias, class JoinConditionType>
    friend auto operator|(const SelectFrom& s, const InnerJoin<JoinTableType, JoinAlias, JoinConditionType>& j) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call join() after where()");
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call join() after group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call join() after having()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call join() after order_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call join() after limit()");

        using NewJoinType = typename InnerJoin<JoinTableType, JoinAlias, JoinConditionType>::JoinType;
        if constexpr (std::is_same_v<JoinListType, Nothing>) {
            // First JOIN
            using NewJoinListType = transpilation::JoinList<NewJoinType>;
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = NewJoinListType{j.join_clause},
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        } else {
            // Append to existing JOINs
            auto new_joins = std::tuple_cat(s.joins_.joins, std::make_tuple(j.join_clause));
            using NewJoinListType = decltype(std::apply([](auto&&... joins) {
                return transpilation::JoinList{joins...};
            }, new_joins));
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = std::apply([](auto&&... joins) {
                    return transpilation::JoinList{joins...};
                }, new_joins),
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        }
    }

    /// Pipe operator for LEFT JOIN
    template <class JoinTableType, glz::string_literal JoinAlias, class JoinConditionType>
    friend auto operator|(const SelectFrom& s, const LeftJoin<JoinTableType, JoinAlias, JoinConditionType>& j) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call join() after where()");
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call join() after group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call join() after having()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call join() after order_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call join() after limit()");

        using NewJoinType = typename LeftJoin<JoinTableType, JoinAlias, JoinConditionType>::JoinType;
        if constexpr (std::is_same_v<JoinListType, Nothing>) {
            using NewJoinListType = transpilation::JoinList<NewJoinType>;
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = NewJoinListType{j.join_clause},
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        } else {
            auto new_joins = std::tuple_cat(s.joins_.joins, std::make_tuple(j.join_clause));
            using NewJoinListType = decltype(std::apply([](auto&&... joins) {
                return transpilation::JoinList{joins...};
            }, new_joins));
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = std::apply([](auto&&... joins) {
                    return transpilation::JoinList{joins...};
                }, new_joins),
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        }
    }

    /// Pipe operator for RIGHT JOIN
    template <class JoinTableType, glz::string_literal JoinAlias, class JoinConditionType>
    friend auto operator|(const SelectFrom& s, const RightJoin<JoinTableType, JoinAlias, JoinConditionType>& j) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call join() after where()");
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call join() after group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call join() after having()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call join() after order_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call join() after limit()");

        using NewJoinType = typename RightJoin<JoinTableType, JoinAlias, JoinConditionType>::JoinType;
        if constexpr (std::is_same_v<JoinListType, Nothing>) {
            using NewJoinListType = transpilation::JoinList<NewJoinType>;
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = NewJoinListType{j.join_clause},
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        } else {
            auto new_joins = std::tuple_cat(s.joins_.joins, std::make_tuple(j.join_clause));
            using NewJoinListType = decltype(std::apply([](auto&&... joins) {
                return transpilation::JoinList{joins...};
            }, new_joins));
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = std::apply([](auto&&... joins) {
                    return transpilation::JoinList{joins...};
                }, new_joins),
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        }
    }

    /// Pipe operator for FULL JOIN
    template <class JoinTableType, glz::string_literal JoinAlias, class JoinConditionType>
    friend auto operator|(const SelectFrom& s, const FullJoin<JoinTableType, JoinAlias, JoinConditionType>& j) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call join() after where()");
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call join() after group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call join() after having()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call join() after order_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call join() after limit()");

        using NewJoinType = typename FullJoin<JoinTableType, JoinAlias, JoinConditionType>::JoinType;
        if constexpr (std::is_same_v<JoinListType, Nothing>) {
            using NewJoinListType = transpilation::JoinList<NewJoinType>;
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = NewJoinListType{j.join_clause},
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        } else {
            auto new_joins = std::tuple_cat(s.joins_.joins, std::make_tuple(j.join_clause));
            using NewJoinListType = decltype(std::apply([](auto&&... joins) {
                return transpilation::JoinList{joins...};
            }, new_joins));
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = std::apply([](auto&&... joins) {
                    return transpilation::JoinList{joins...};
                }, new_joins),
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        }
    }

    /// Pipe operator for CROSS JOIN
    template <class JoinTableType, glz::string_literal JoinAlias>
    friend auto operator|(const SelectFrom& s, const CrossJoin<JoinTableType, JoinAlias>& j) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call join() after where()");
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call join() after group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call join() after having()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call join() after order_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call join() after limit()");

        using NewJoinType = typename CrossJoin<JoinTableType, JoinAlias>::JoinType;
        if constexpr (std::is_same_v<JoinListType, Nothing>) {
            using NewJoinListType = transpilation::JoinList<NewJoinType>;
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = NewJoinListType{j.join_clause},
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        } else {
            auto new_joins = std::tuple_cat(s.joins_.joins, std::make_tuple(j.join_clause));
            using NewJoinListType = decltype(std::apply([](auto&&... joins) {
                return transpilation::JoinList{joins...};
            }, new_joins));
            return SelectFrom<TableType, FieldsTuple, NewJoinListType, WhereType, GroupByType, HavingType, OrderByType, LimitType>{
                .fields_ = s.fields_,
                .joins_ = std::apply([](auto&&... joins) {
                    return transpilation::JoinList{joins...};
                }, new_joins),
                .where_ = s.where_,
                .group_by_ = s.group_by_,
                .having_ = s.having_,
                .order_by_ = s.order_by_,
                .limit_ = s.limit_
            };
        }
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

        return SelectFrom<TableType, FieldsTuple, JoinListType, ConditionType, GroupByType, HavingType, OrderByType, LimitType>{
            .fields_ = s.fields_,
            .joins_ = s.joins_,
            .where_ = w.condition,
            .group_by_ = s.group_by_,
            .having_ = s.having_,
            .order_by_ = s.order_by_,
            .limit_ = s.limit_
        };
    }


    /// Pipe operator for GROUP BY clause
    template <class... ColTypes>
    friend auto operator|(const SelectFrom& s, const GroupBy<ColTypes...>& g) {
        static_assert(std::is_same_v<GroupByType, Nothing>,
                     "Cannot call group_by() twice");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call having() before group_by()");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call order_by() before group_by()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() before group_by()");

        return SelectFrom<TableType, FieldsTuple, JoinListType, WhereType, GroupBy<ColTypes...>, HavingType, OrderByType, LimitType>{
            .fields_ = s.fields_,
            .joins_ = s.joins_,
            .where_ = s.where_,
            .group_by_ = g,
            .having_ = s.having_,
            .order_by_ = s.order_by_,
            .limit_ = s.limit_
        };
    }

    /// Pipe operator for HAVING clause
    template <class ConditionType>
    friend auto operator|(const SelectFrom& s, const Having<ConditionType>& h) {
        static_assert(!std::is_same_v<GroupByType, Nothing>,
                     "Cannot call having() without group_by()");
        static_assert(std::is_same_v<HavingType, Nothing>,
                     "Cannot call having() twice");
        static_assert(std::is_same_v<OrderByType, Nothing>,
                     "Cannot call order_by() before having()");
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() before having()");

        return SelectFrom<TableType, FieldsTuple, JoinListType, WhereType, GroupByType, ConditionType, OrderByType, LimitType>{
            .fields_ = s.fields_,
            .joins_ = s.joins_,
            .where_ = s.where_,
            .group_by_ = s.group_by_,
            .having_ = h.condition,
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

        return SelectFrom<TableType, FieldsTuple, JoinListType, WhereType, GroupByType, HavingType, OrderBy<ColTypes...>, LimitType>{
            .fields_ = s.fields_,
            .joins_ = s.joins_,
            .where_ = s.where_,
            .group_by_ = s.group_by_,
            .having_ = s.having_,
            .order_by_ = o,
            .limit_ = s.limit_
        };
    }

    /// Pipe operator for LIMIT clause
    friend auto operator|(const SelectFrom& s, const Limit& l) {
        static_assert(std::is_same_v<LimitType, Nothing>,
                     "Cannot call limit() twice");

        return SelectFrom<TableType, FieldsTuple, JoinListType, WhereType, GroupByType, HavingType, OrderByType, Limit>{
            .fields_ = s.fields_,
            .joins_ = s.joins_,
            .where_ = s.where_,
            .group_by_ = s.group_by_,
            .having_ = s.having_,
            .order_by_ = s.order_by_,
            .limit_ = l
        };
    }

    FieldsTuple fields_;
    JoinListType joins_;
    WhereType where_;
    GroupByType group_by_;
    HavingType having_;
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
        .fields_ = std::make_tuple(fields...),
        .joins_ = Nothing{},
        .where_ = Nothing{},
        .group_by_ = Nothing{},
        .having_ = Nothing{},
        .order_by_ = Nothing{},
        .limit_ = Nothing{}
    };
}

// ============================================================================
// INSERT Query Builder
// ============================================================================

/// INSERT query builder
template <class TableType>
struct Insert {
    /// Convert to SQL string (returns statement with placeholders)
    std::string to_sql() const {
        std::string sql = or_replace_ ? "INSERT OR REPLACE INTO " : "INSERT INTO ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());
        sql += " (";
        sql += transpilation::insert_field_list<TableType>();
        sql += ") VALUES (";
        sql += transpilation::insert_placeholders<TableType>();
        sql += ")";

        return sql;
    }

    bool or_replace_ = false;
};

/// Create an INSERT INTO Table query
template <class TableType>
auto insert() {
    return Insert<TableType>{.or_replace_ = false};
}

/// Create an INSERT OR REPLACE INTO Table query
template <class TableType>
auto insert_or_replace() {
    return Insert<TableType>{.or_replace_ = true};
}

// ============================================================================
// UPDATE Query Builder
// ============================================================================

/// UPDATE query builder
template <class TableType, class SetsTuple, class WhereType = Nothing>
struct Update {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "UPDATE ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());
        sql += " SET ";

        // Add SET clauses
        bool first = true;
        std::apply([&](const auto&... sets) {
            (([&] {
                if (first) {
                    sql += transpilation::to_sql(sets);
                    first = false;
                } else {
                    sql += ", " + transpilation::to_sql(sets);
                }
            }()), ...);
        }, sets_);

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        return sql;
    }

    /// Pipe operator for WHERE clause
    template <class ConditionType>
    friend auto operator|(const Update& u, const Where<ConditionType>& w) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call where() twice");

        return Update<TableType, SetsTuple, ConditionType>{
            .sets_ = u.sets_,
            .where_ = w.condition
        };
    }

    SetsTuple sets_;
    WhereType where_;
};

/// Create an UPDATE Table SET ... query
template <class TableType, class... SetTypes>
auto update(const SetTypes&... sets) {
    static_assert(sizeof...(sets) > 0, "Must update at least one column");
    using SetsTuple = std::tuple<SetTypes...>;
    return Update<TableType, SetsTuple>{
        .sets_ = std::make_tuple(sets...),
        .where_ = Nothing{}
    };
}

/// Helper function to create SET clauses
template <class Col, class Value>
auto set(const Col& col, const Value& val) {
    return transpilation::make_set(col, transpilation::Value<Value>{val});
}

// ============================================================================
// DELETE Query Builder
// ============================================================================

/// DELETE FROM query builder
template <class TableType, class WhereType = Nothing>
struct DeleteFrom {
    /// Convert to SQL string
    std::string to_sql() const {
        std::string sql = "DELETE FROM ";

        sql += transpilation::quote_identifier(transpilation::get_table_name<TableType>());

        // Add WHERE if specified
        if constexpr (!std::is_same_v<WhereType, Nothing>) {
            sql += " ";
            sql += transpilation::where_clause(where_);
        }

        return sql;
    }

    /// Pipe operator for WHERE clause
    template <class ConditionType>
    friend auto operator|(const DeleteFrom& /*unused*/, const Where<ConditionType>& w) {
        static_assert(std::is_same_v<WhereType, Nothing>,
                     "Cannot call where() twice");

        return DeleteFrom<TableType, ConditionType>{
            .where_ = w.condition
        };
    }

    WhereType where_;
};

/// Create a DELETE FROM Table query
template <class TableType>
auto delete_from() {
    return DeleteFrom<TableType>{};
}

// ============================================================================
// CREATE TABLE Query Builder
// ============================================================================

/// CREATE TABLE query builder
template <class TableType>
struct CreateTable {
    /// Convert to SQL string
    std::string to_sql() const {
        return transpilation::create_table_sql<TableType>(if_not_exists_);
    }

    bool if_not_exists_ = false;
};

/// Create a CREATE TABLE query
template <class TableType>
auto create_table(bool if_not_exists = false) {
    return CreateTable<TableType>{.if_not_exists_ = if_not_exists};
}

} // namespace glz_sqlgen

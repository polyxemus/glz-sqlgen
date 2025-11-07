#include <gtest/gtest.h>
#include <glz_sqlgen/select_from.hpp>
#include <glz_sqlgen/aggregates.hpp>
#include <glz_sqlgen/group_by.hpp>
#include <glz_sqlgen/having.hpp>
#include <glz_sqlgen/where.hpp>
#include <glz_sqlgen/order_by.hpp>
#include <glz_sqlgen/limit.hpp>
#include <glz_sqlgen/literals.hpp>

namespace {

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

// Test tables
struct Sales {
    int id;
    std::string product;
    double amount;
    int quantity;
    std::string region;
};

struct Employees {
    int id;
    std::string name;
    int department_id;
    double salary;
};

} // namespace

// Glaze metadata
template <>
struct glz::meta<Sales> {
    using T = Sales;
    static constexpr std::string_view name = "sales";
    static constexpr auto value = object(
        "id", &T::id,
        "product", &T::product,
        "amount", &T::amount,
        "quantity", &T::quantity,
        "region", &T::region
    );
};

template <>
struct glz::meta<Employees> {
    using T = Employees;
    static constexpr std::string_view name = "employees";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "department_id", &T::department_id,
        "salary", &T::salary
    );
};

// === Aggregate Function Tests ===

TEST(AggregatesTest, CountStar) {
    auto query = select_from<Sales>(count_star());

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COUNT(*) FROM \"sales\"");
}

TEST(AggregatesTest, CountColumn) {
    auto query = select_from<Sales>(count("product"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COUNT(\"product\") FROM \"sales\"");
}

TEST(AggregatesTest, CountDistinct) {
    auto query = select_from<Sales>(count_distinct("region"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COUNT(DISTINCT \"region\") FROM \"sales\"");
}

TEST(AggregatesTest, Sum) {
    auto query = select_from<Sales>(sum("amount"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT SUM(\"amount\") FROM \"sales\"");
}

TEST(AggregatesTest, Avg) {
    auto query = select_from<Sales>(avg("amount"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT AVG(\"amount\") FROM \"sales\"");
}

TEST(AggregatesTest, Min) {
    auto query = select_from<Sales>(min("amount"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT MIN(\"amount\") FROM \"sales\"");
}

TEST(AggregatesTest, Max) {
    auto query = select_from<Sales>(max("amount"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT MAX(\"amount\") FROM \"sales\"");
}

TEST(AggregatesTest, MultipleAggregates) {
    auto query = select_from<Sales>(count_star(), sum("amount"_c), avg("quantity"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COUNT(*), SUM(\"amount\"), AVG(\"quantity\") FROM \"sales\"");
}

// === GROUP BY Tests ===

TEST(AggregatesTest, GroupBySingleColumn) {
    auto query = select_from<Sales>("region"_c, sum("amount"_c)) |
                 group_by("region"_c");

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", SUM(\"amount\") FROM \"sales\" GROUP BY \"region\"");
}

TEST(AggregatesTest, GroupByMultipleColumns) {
    auto query = select_from<Sales>("region"_c, "product"_c, sum("amount"_c)) |
                 group_by("region"_c, "product"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", \"product\", SUM(\"amount\") "
                   "FROM \"sales\" GROUP BY \"region\", \"product\"");
}

TEST(AggregatesTest, GroupByWithWhere) {
    auto query = select_from<Sales>("region"_c, count_star()) |
                 where("amount"_c > 100.0) |
                 group_by("region"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", COUNT(*) FROM \"sales\" "
                   "WHERE \"amount\" > 100 GROUP BY \"region\"");
}

TEST(AggregatesTest, GroupByWithOrderBy) {
    auto query = select_from<Sales>("region"_c, sum("amount"_c)) |
                 group_by("region"_c) |
                 order_by("region"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", SUM(\"amount\") FROM \"sales\" "
                   "GROUP BY \"region\" ORDER BY \"region\"");
}

TEST(AggregatesTest, GroupByWithLimit) {
    auto query = select_from<Sales>("region"_c, sum("amount"_c)) |
                 group_by("region"_c) |
                 limit(10);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", SUM(\"amount\") FROM \"sales\" "
                   "GROUP BY \"region\" LIMIT 10");
}

// === HAVING Tests ===

TEST(AggregatesTest, HavingSimple) {
    auto query = select_from<Sales>("region"_c, sum("amount"_c)) |
                 group_by("region"_c) |
                 having(sum("amount"_c) > 1000.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", SUM(\"amount\") FROM \"sales\" "
                   "GROUP BY \"region\" HAVING SUM(\"amount\") > 1000");
}

TEST(AggregatesTest, HavingWithCount) {
    auto query = select_from<Sales>("region"_c, count_star()) |
                 group_by("region"_c) |
                 having(count_star() >= 5);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", COUNT(*) FROM \"sales\" "
                   "GROUP BY \"region\" HAVING COUNT(*) >= 5");
}

TEST(AggregatesTest, HavingWithWhereAndOrderBy) {
    auto query = select_from<Sales>("region"_c, avg("amount"_c)) |
                 where("quantity"_c > 0) |
                 group_by("region"_c) |
                 having(avg("amount"_c) > 50.0) |
                 order_by("region"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"region\", AVG(\"amount\") FROM \"sales\" "
                   "WHERE \"quantity\" > 0 "
                   "GROUP BY \"region\" "
                   "HAVING AVG(\"amount\") > 50 "
                   "ORDER BY \"region\"");
}

// === Complex Query Tests ===

TEST(AggregatesTest, CompleteAnalyticsQuery) {
    auto query = select_from<Sales>("region"_c, "product"_c,
                                     count_star(), sum("amount"_c), avg("quantity"_c)) |
                 where("amount"_c > 10.0) |
                 group_by("region"_c, "product"_c) |
                 having(count_star() > 5) |
                 order_by("region"_c, sum("amount"_c).desc()) |
                 limit(20);

    auto sql = query.to_sql();
    // This is a complex query, verify it has all the clauses
    EXPECT_TRUE(sql.find("SELECT") != std::string::npos);
    EXPECT_TRUE(sql.find("COUNT(*)") != std::string::npos);
    EXPECT_TRUE(sql.find("SUM") != std::string::npos);
    EXPECT_TRUE(sql.find("AVG") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"sales\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE") != std::string::npos);
    EXPECT_TRUE(sql.find("GROUP BY") != std::string::npos);
    EXPECT_TRUE(sql.find("HAVING") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY") != std::string::npos);
    EXPECT_TRUE(sql.find("LIMIT") != std::string::npos);
}

TEST(AggregatesTest, DepartmentSalaryStats) {
    auto query = select_from<Employees>("department_id"_c,
                                         count_star(),
                                         min("salary"_c),
                                         max("salary"_c),
                                         avg("salary"_c)) |
                 group_by("department_id"_c) |
                 having(count_star() >= 3);

    auto sql = query.to_sql();
    EXPECT_TRUE(sql.find("COUNT(*)") != std::string::npos);
    EXPECT_TRUE(sql.find("MIN(\"salary\")") != std::string::npos);
    EXPECT_TRUE(sql.find("MAX(\"salary\")") != std::string::npos);
    EXPECT_TRUE(sql.find("AVG(\"salary\")") != std::string::npos);
    EXPECT_TRUE(sql.find("GROUP BY \"department_id\"") != std::string::npos);
    EXPECT_TRUE(sql.find("HAVING COUNT(*) >= 3") != std::string::npos);
}

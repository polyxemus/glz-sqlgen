#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include "glz_sqlgen/literals.hpp"
#include "glz_sqlgen/transpilation/to_sql_string.hpp"
#include "glz_sqlgen/transpilation/where_clause.hpp"

using namespace glz_sqlgen::literals;
namespace t = glz_sqlgen::transpilation;

TEST(SqlGenerationTest, ColumnToSql) {
    t::Col<"name"> col;
    auto sql = t::to_sql(col);
    EXPECT_EQ(sql, "\"name\"");
}

TEST(SqlGenerationTest, ColumnWithAliasToSql) {
    t::Col<"name", "t1"> col;
    auto sql = t::to_sql(col);
    EXPECT_EQ(sql, "\"t1\".\"name\"");
}

TEST(SqlGenerationTest, IntValueToSql) {
    t::Value<int> val{42};
    auto sql = t::to_sql(val);
    EXPECT_EQ(sql, "42");
}

TEST(SqlGenerationTest, StringValueToSql) {
    t::Value<std::string> val{"hello"};
    auto sql = t::to_sql(val);
    EXPECT_EQ(sql, "'hello'");
}

TEST(SqlGenerationTest, StringWithQuotesToSql) {
    t::Value<std::string> val{"it's"};
    auto sql = t::to_sql(val);
    EXPECT_EQ(sql, "'it''s'");  // SQL escapes single quotes
}

TEST(SqlGenerationTest, BoolValueToSql) {
    t::Value<bool> val_true{true};
    t::Value<bool> val_false{false};
    EXPECT_EQ(t::to_sql(val_true), "1");
    EXPECT_EQ(t::to_sql(val_false), "0");
}

TEST(SqlGenerationTest, SimpleComparison) {
    auto condition = "age"_c == 18;
    auto sql = t::to_sql(condition);
    EXPECT_EQ(sql, "\"age\" = 18");
}

TEST(SqlGenerationTest, GreaterThanComparison) {
    auto condition = "price"_c > 100;
    auto sql = t::to_sql(condition);
    EXPECT_EQ(sql, "\"price\" > 100");
}

TEST(SqlGenerationTest, LessThanOrEqual) {
    auto condition = "score"_c <= 50;
    auto sql = t::to_sql(condition);
    EXPECT_EQ(sql, "\"score\" <= 50");
}

TEST(SqlGenerationTest, ColumnToColumnComparison) {
    auto condition = "price"_t1 == "cost"_t2;
    auto sql = t::to_sql(condition);
    EXPECT_EQ(sql, "\"t1\".\"price\" = \"t2\".\"cost\"");
}

TEST(SqlGenerationTest, SimpleArithmetic) {
    auto expr = "price"_c + 10;
    auto sql = t::to_sql(expr);
    EXPECT_EQ(sql, "(\"price\" + 10)");
}

TEST(SqlGenerationTest, Multiplication) {
    auto expr = "quantity"_c * 5;
    auto sql = t::to_sql(expr);
    EXPECT_EQ(sql, "(\"quantity\" * 5)");
}

TEST(SqlGenerationTest, Division) {
    auto expr = "total"_c / 2;
    auto sql = t::to_sql(expr);
    EXPECT_EQ(sql, "(\"total\" / 2)");
}

TEST(SqlGenerationTest, ColumnToColumnArithmetic) {
    auto expr = "price"_c + "tax"_c;
    auto sql = t::to_sql(expr);
    EXPECT_EQ(sql, "(\"price\" + \"tax\")");
}

TEST(SqlGenerationTest, ChainedArithmetic) {
    auto expr = ("price"_c + "tax"_c) * 2;
    auto sql = t::to_sql(expr);
    EXPECT_EQ(sql, "((\"price\" + \"tax\") * 2)");
}

TEST(SqlGenerationTest, ComplexExpression) {
    auto expr = ("price"_c - "discount"_c) * 1.1;
    auto sql = t::to_sql(expr);
    // Note: floating point will have some representation
    EXPECT_TRUE(sql.find("(\"price\" - \"discount\")") != std::string::npos);
    EXPECT_TRUE(sql.find("1.1") != std::string::npos);
}

TEST(SqlGenerationTest, WhereClauseSimple) {
    auto condition = "age"_c >= 18;
    auto sql = t::where_clause(condition);
    EXPECT_EQ(sql, "WHERE \"age\" >= 18");
}

TEST(SqlGenerationTest, WhereClauseString) {
    auto condition = "name"_c == "John";
    auto sql = t::where_clause(condition);
    EXPECT_EQ(sql, "WHERE \"name\" = 'John'");
}

TEST(SqlGenerationTest, WhereClauseWithAlias) {
    auto condition = "id"_t1 == "parent_id"_t2;
    auto sql = t::where_clause(condition);
    EXPECT_EQ(sql, "WHERE \"t1\".\"id\" = \"t2\".\"parent_id\"");
}

TEST(SqlGenerationTest, ComplexWhereCondition) {
    auto condition = ("price"_c + "tax"_c) > 100;
    auto sql = t::where_clause(condition);
    EXPECT_EQ(sql, "WHERE (\"price\" + \"tax\") > 100");
}

TEST(SqlGenerationTest, OperatorSymbols) {
    EXPECT_EQ(t::operator_to_sql(t::Operator::equal), " = ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::not_equal), " != ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::less_than), " < ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::less_equal), " <= ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::greater_than), " > ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::greater_equal), " >= ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::plus), " + ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::minus), " - ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::multiplies), " * ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::divides), " / ");
    EXPECT_EQ(t::operator_to_sql(t::Operator::mod), " % ");
}

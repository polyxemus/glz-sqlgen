#include <gtest/gtest.h>
#include "glz_sqlgen/literals.hpp"

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

TEST(LiteralsTest, BasicColumnLiteral) {
    auto col = "name"_c;
    EXPECT_EQ(col.get_name(), "name");
}

TEST(LiteralsTest, TableAliasLiterals) {
    auto col_t1 = "name"_t1;
    EXPECT_EQ(col_t1.get_name(), "name");
    EXPECT_EQ(col_t1.alias, "t1");

    auto col_t2 = "age"_t2;
    EXPECT_EQ(col_t2.get_name(), "age");
    EXPECT_EQ(col_t2.alias, "t2");

    auto col_t99 = "id"_t99;
    EXPECT_EQ(col_t99.get_name(), "id");
    EXPECT_EQ(col_t99.alias, "t99");
}

TEST(LiteralsTest, UsageInExpressions) {
    // Test that literals work in expressions
    auto condition = "age"_c >= 18;
    auto operation = "price"_c + 10;

    // Just verify they compile
    (void)condition;
    (void)operation;
}

TEST(LiteralsTest, JoinWithAliases) {
    // Test table aliases work
    auto join_condition = "id"_t1 == "parent_id"_t2;

    // Just verify it compiles
    (void)join_condition;
}

TEST(LiteralsTest, MultipleOperations) {
    // Chain operations
    auto expr = ("price"_c + "tax"_c) * 2;

    // Verify it compiles
    (void)expr;
}

TEST(LiteralsTest, ComplexConditions) {
    // Build more complex expressions
    auto price = "price"_c;
    auto discount = "discount"_c;
    auto final_price = (price - discount) * 1.1; // Price after discount + 10% tax

    (void)final_price;
}

TEST(LiteralsTest, AliasedColumns) {
    // Test aliasing
    auto col1 = "name"_t1;
    auto col2 = "name"_t2;

    // These should be different columns from different tables
    EXPECT_EQ(col1.alias, "t1");
    EXPECT_EQ(col2.alias, "t2");
}

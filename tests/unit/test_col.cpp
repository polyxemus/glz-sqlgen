#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include "glz_sqlgen/col.hpp"
#include "glz_sqlgen/transpilation/Col.hpp"

using namespace glz_sqlgen;

TEST(ColTest, BasicCreation) {
    constexpr Col<"name"> col;
    EXPECT_EQ(col.get_name(), "name");
}

TEST(ColTest, WithAlias) {
    constexpr Col<"name", "t1"> col;
    EXPECT_EQ(col.get_name(), "name");
    EXPECT_EQ(col.alias, "t1");
}

TEST(ColTest, Aliasing) {
    constexpr Col<"name"> col;
    auto aliased = col.as<"new_name">();
    EXPECT_EQ(decltype(aliased)::alias_name, "new_name");
}

TEST(ColTest, DescendingOrder) {
    constexpr Col<"name"> col;
    auto desc_col = col.desc();
    // Just verify it compiles
    (void)desc_col;
}

TEST(ColTest, ComparisonOperators) {
    constexpr Col<"age"> age_col;

    // Test that these compile
    auto eq = age_col == 18;
    auto ne = age_col != 18;
    auto lt = age_col < 18;
    auto le = age_col <= 18;
    auto gt = age_col > 18;
    auto ge = age_col >= 18;

    // Just verify they compile
    (void)eq; (void)ne; (void)lt; (void)le; (void)gt; (void)ge;
}

TEST(ColTest, ArithmeticOperators) {
    constexpr Col<"price"> price_col;

    // Test that these compile
    auto add = price_col + 10;
    auto sub = price_col - 5;
    auto mul = price_col * 2;
    auto div = price_col / 2;
    auto mod = price_col % 3;

    // Just verify they compile
    (void)add; (void)sub; (void)mul; (void)div; (void)mod;
}

TEST(ColTest, ColumnToColumnComparison) {
    constexpr Col<"price"> price;
    constexpr Col<"cost"> cost;

    auto cmp_eq = price == cost;
    auto cmp_ne = price != cost;

    // Just verify they compile
    (void)cmp_eq; (void)cmp_ne;
}

TEST(ColTest, ColumnToColumnArithmetic) {
    constexpr Col<"price"> price;
    constexpr Col<"tax"> tax;

    auto total = price + tax;
    auto diff = price - tax;
    auto product = price * tax;
    auto ratio = price / tax;

    // Just verify they compile
    (void)total; (void)diff; (void)product; (void)ratio;
}

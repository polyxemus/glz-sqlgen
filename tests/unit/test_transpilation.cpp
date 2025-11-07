#include <gtest/gtest.h>
#include "glz_sqlgen/transpilation/Col.hpp"
#include "glz_sqlgen/transpilation/Value.hpp"
#include "glz_sqlgen/transpilation/Operator.hpp"
#include "glz_sqlgen/transpilation/Operation.hpp"
#include "glz_sqlgen/transpilation/Condition.hpp"
#include "glz_sqlgen/transpilation/As.hpp"
#include "glz_sqlgen/transpilation/Desc.hpp"
#include "glz_sqlgen/transpilation/to_sql_type.hpp"
#include "glz_sqlgen/transpilation/quote.hpp"

using namespace glz_sqlgen::transpilation;

TEST(TranspilationTest, ColBasic) {
    constexpr Col<"name"> col;
    EXPECT_EQ(col.name, "name");
    EXPECT_EQ(col.alias, "");
    EXPECT_FALSE(col.has_alias());
}

TEST(TranspilationTest, ColWithAlias) {
    constexpr Col<"name", "t1"> col;
    EXPECT_EQ(col.name, "name");
    EXPECT_EQ(col.alias, "t1");
    EXPECT_TRUE(col.has_alias());
}

TEST(TranspilationTest, ValueTypes) {
    Value<int> int_val{42};
    EXPECT_EQ(int_val.get(), 42);

    Value<std::string> str_val{"hello"};
    EXPECT_EQ(str_val.get(), "hello");

    Value<double> double_val{3.14};
    EXPECT_DOUBLE_EQ(double_val.get(), 3.14);
}

TEST(TranspilationTest, OperationCreation) {
    auto op = make_operation<Operator::plus>(Col<"price">{}, Value<int>{10});
    EXPECT_EQ(op.op, Operator::plus);
    EXPECT_EQ(op.operand2.get(), 10);
}

TEST(TranspilationTest, ConditionCreation) {
    auto cond = make_condition<Operator::equal>(Col<"age">{}, Value<int>{18});
    EXPECT_EQ(cond.op, Operator::equal);
    EXPECT_EQ(cond.right.get(), 18);
}

TEST(TranspilationTest, AsAliasing) {
    auto aliased = make_as<Col<"old_name">, "new_name">(Col<"old_name">{});
    EXPECT_EQ(aliased.get_alias(), "new_name");
}

TEST(TranspilationTest, DescOrdering) {
    auto desc = make_desc(Col<"created_at">{});
    EXPECT_EQ(desc.get_column().name, "created_at");
}

TEST(TranspilationTest, SqlTypeMapping) {
    EXPECT_EQ(to_sql_type<int>(), "INTEGER");
    EXPECT_EQ(to_sql_type<int64_t>(), "INTEGER");
    EXPECT_EQ(to_sql_type<double>(), "REAL");
    EXPECT_EQ(to_sql_type<float>(), "REAL");
    EXPECT_EQ(to_sql_type<std::string>(), "TEXT");
    EXPECT_EQ(to_sql_type<bool>(), "INTEGER");
}

TEST(TranspilationTest, OptionalDetection) {
    EXPECT_FALSE(is_optional_v<int>);
    EXPECT_FALSE(is_optional_v<std::string>);
    EXPECT_TRUE(is_optional_v<std::optional<int>>);
    EXPECT_TRUE(is_optional_v<std::optional<std::string>>);
}

TEST(TranspilationTest, QuoteIdentifier) {
    EXPECT_EQ(quote_identifier("name"), "\"name\"");
    EXPECT_EQ(quote_identifier("table_name"), "\"table_name\"");
}

TEST(TranspilationTest, QuoteString) {
    EXPECT_EQ(quote_string("hello"), "'hello'");
    EXPECT_EQ(quote_string("it's"), "'it''s'"); // SQL escapes single quotes
    EXPECT_EQ(quote_string("test"), "'test'");
}

TEST(TranspilationTest, ComplexExpression) {
    // Build: (price + tax) * quantity
    auto price = Col<"price">{};
    auto tax = Col<"tax">{};
    auto quantity = Value<int>{5};

    auto sum = make_operation<Operator::plus>(price, tax);
    auto result = make_operation<Operator::multiplies>(sum, quantity);

    // Verify types
    static_assert(std::is_same_v<
        decltype(result),
        Operation<Operator::multiplies,
            Operation<Operator::plus, Col<"price">, Col<"tax">>,
            Value<int>>
    >);
}

TEST(TranspilationTest, ComplexCondition) {
    // Build: age >= 18 AND age <= 65
    auto age = Col<"age">{};

    auto cond1 = make_condition<Operator::greater_equal>(age, Value<int>{18});
    auto cond2 = make_condition<Operator::less_equal>(age, Value<int>{65});

    // Verify individual conditions compile
    EXPECT_EQ(cond1.op, Operator::greater_equal);
    EXPECT_EQ(cond2.op, Operator::less_equal);
}

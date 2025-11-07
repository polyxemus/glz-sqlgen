#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <glz_sqlgen/core.hpp>
#include <glz_sqlgen/query_builders.hpp>
#include <glz_sqlgen/query_clauses.hpp>
#include <glz_sqlgen/functions.hpp>

namespace {

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

// Test tables
struct Products {
    int id;
    std::string name;
    double price;
    double cost;
    int quantity;
};

struct Measurements {
    int id;
    double x;
    double y;
    double angle;
    double value;
};

} // namespace

// Glaze metadata
template <>
struct glz::meta<Products> {
    using T = Products;
    static constexpr std::string_view name = "products";
    [[maybe_unused]] static constexpr auto value = glz::object(
        "id", &T::id,
        "name", &T::name,
        "price", &T::price,
        "cost", &T::cost,
        "quantity", &T::quantity
    );
};

template <>
struct glz::meta<Measurements> {
    using T = Measurements;
    static constexpr std::string_view name = "measurements";
    [[maybe_unused]] static constexpr auto value = glz::object(
        "id", &T::id,
        "x", &T::x,
        "y", &T::y,
        "angle", &T::angle,
        "value", &T::value
    );
};

// ABS Tests

TEST(MathFunctionsTest, AbsColumn) {
    auto query = select_from<Measurements>(abs("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ABS(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, AbsInWhere) {
    auto query = select_from<Measurements>()
        | where(abs("value"_c) > 10.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE ABS(\"value\") > 10");
}

TEST(MathFunctionsTest, AbsExpression) {
    auto query = select_from<Products>(abs("price"_c - "cost"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ABS((\"price\" - \"cost\")) FROM \"products\"");
}

// CEIL Tests

TEST(MathFunctionsTest, CeilColumn) {
    auto query = select_from<Products>(ceil("price"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CEIL(\"price\") FROM \"products\"");
}

TEST(MathFunctionsTest, CeilInWhere) {
    auto query = select_from<Products>()
        | where(ceil("price"_c) >= 100);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"name\", \"price\", \"cost\", \"quantity\" FROM \"products\" WHERE CEIL(\"price\") >= 100");
}

// FLOOR Tests

TEST(MathFunctionsTest, FloorColumn) {
    auto query = select_from<Products>(floor("price"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT FLOOR(\"price\") FROM \"products\"");
}

TEST(MathFunctionsTest, FloorInWhere) {
    auto query = select_from<Products>()
        | where(floor("price"_c) <= 50);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"name\", \"price\", \"cost\", \"quantity\" FROM \"products\" WHERE FLOOR(\"price\") <= 50");
}

// ROUND Tests

TEST(MathFunctionsTest, RoundNoDecimal) {
    auto query = select_from<Products>(round("price"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ROUND(\"price\") FROM \"products\"");
}

TEST(MathFunctionsTest, RoundWithPrecision) {
    auto query = select_from<Products>(round("price"_c, 2));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ROUND(\"price\", 2) FROM \"products\"");
}

TEST(MathFunctionsTest, RoundInWhere) {
    auto query = select_from<Products>()
        | where(round("price"_c, 0) == 100);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"name\", \"price\", \"cost\", \"quantity\" FROM \"products\" WHERE ROUND(\"price\", 0) = 100");
}

// SQRT Tests

TEST(MathFunctionsTest, SqrtColumn) {
    auto query = select_from<Measurements>(sqrt("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT SQRT(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, SqrtInWhere) {
    auto query = select_from<Measurements>()
        | where(sqrt("value"_c) > 5.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE SQRT(\"value\") > 5");
}

TEST(MathFunctionsTest, SqrtExpression) {
    auto query = select_from<Measurements>(
        sqrt("x"_c * "x"_c + "y"_c * "y"_c)
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT SQRT(((\"x\" * \"x\") + (\"y\" * \"y\"))) FROM \"measurements\"");
}

// EXP Tests

TEST(MathFunctionsTest, ExpColumn) {
    auto query = select_from<Measurements>(exp("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT EXP(\"value\") FROM \"measurements\"");
}

// LN Tests

TEST(MathFunctionsTest, LnColumn) {
    auto query = select_from<Measurements>(ln("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT LN(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, LnInWhere) {
    auto query = select_from<Measurements>()
        | where(ln("value"_c) > 2.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE LN(\"value\") > 2");
}

// LOG2 Tests

TEST(MathFunctionsTest, Log2Column) {
    auto query = select_from<Measurements>(log2("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT LOG2(\"value\") FROM \"measurements\"");
}

// LOG10 Tests

TEST(MathFunctionsTest, Log10Column) {
    auto query = select_from<Measurements>(log10("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT LOG10(\"value\") FROM \"measurements\"");
}

// POW Tests

TEST(MathFunctionsTest, PowColumn) {
    auto query = select_from<Measurements>(pow("x"_c, 2));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT POW(\"x\", 2) FROM \"measurements\"");
}

TEST(MathFunctionsTest, PowTwoColumns) {
    auto query = select_from<Measurements>(pow("x"_c, "y"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT POW(\"x\", \"y\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, PowInWhere) {
    auto query = select_from<Measurements>()
        | where(pow("x"_c, 2) > 100.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE POW(\"x\", 2) > 100");
}

// TRIGONOMETRIC FUNCTIONS

TEST(MathFunctionsTest, SinColumn) {
    auto query = select_from<Measurements>(sin("angle"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT SIN(\"angle\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, CosColumn) {
    auto query = select_from<Measurements>(cos("angle"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COS(\"angle\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, TanColumn) {
    auto query = select_from<Measurements>(tan("angle"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT TAN(\"angle\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, AsinColumn) {
    auto query = select_from<Measurements>(asin("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ASIN(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, AcosColumn) {
    auto query = select_from<Measurements>(acos("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ACOS(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, AtanColumn) {
    auto query = select_from<Measurements>(atan("value"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ATAN(\"value\") FROM \"measurements\"");
}

TEST(MathFunctionsTest, TrigInWhere) {
    auto query = select_from<Measurements>()
        | where(sin("angle"_c) > 0.5);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE SIN(\"angle\") > 0.5");
}

// COMBINED MATH FUNCTIONS

TEST(MathFunctionsTest, CombinedMathFunctions) {
    auto query = select_from<Measurements>(
        round(sqrt(pow("x"_c, 2) + pow("y"_c, 2)), 2)
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ROUND(SQRT((POW(\"x\", 2) + POW(\"y\", 2))), 2) FROM \"measurements\"");
}

TEST(MathFunctionsTest, ComplexExpression) {
    auto query = select_from<Products>(
        round(("price"_c - "cost"_c) / "price"_c * 100, 2)
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ROUND((((\"price\" - \"cost\") / \"price\") * 100), 2) FROM \"products\"");
}

TEST(MathFunctionsTest, NestedMathFunctions) {
    auto query = select_from<Measurements>(
        abs(ceil(floor("value"_c)))
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT ABS(CEIL(FLOOR(\"value\"))) FROM \"measurements\"");
}

TEST(MathFunctionsTest, MathFunctionsWithOrderBy) {
    auto query = select_from<Products>()
        | where(abs("price"_c - "cost"_c) > 10.0)
        | order_by(round("price"_c, 0));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"name\", \"price\", \"cost\", \"quantity\" FROM \"products\" WHERE ABS((\"price\" - \"cost\")) > 10 ORDER BY ROUND(\"price\", 0)");
}

TEST(MathFunctionsTest, PythagoreanTheorem) {
    auto query = select_from<Measurements>()
        | where(sqrt(pow("x"_c, 2) + pow("y"_c, 2)) < 10.0);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"x\", \"y\", \"angle\", \"value\" FROM \"measurements\" WHERE SQRT((POW(\"x\", 2) + POW(\"y\", 2))) < 10");
}

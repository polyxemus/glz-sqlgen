#include <gtest/gtest.h>
#include <glz_sqlgen/select_from.hpp>
#include <glz_sqlgen/advanced_conditions.hpp>
#include <glz_sqlgen/where.hpp>
#include <glz_sqlgen/order_by.hpp>
#include <glz_sqlgen/limit.hpp>
#include <glz_sqlgen/literals.hpp>

namespace {

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

// Test tables
struct Users {
    int id;
    std::string name;
    std::string email;
    int age;
    std::string city;
};

struct Products {
    int id;
    std::string name;
    double price;
    int stock;
    std::string category;
};

} // namespace

// Glaze metadata
template <>
struct glz::meta<Users> {
    using T = Users;
    static constexpr std::string_view name = "users";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "email", &T::email,
        "age", &T::age,
        "city", &T::city
    );
};

template <>
struct glz::meta<Products> {
    using T = Products;
    static constexpr std::string_view name = "products";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "price", &T::price,
        "stock", &T::stock,
        "category", &T::category
    );
};

// LIKE Pattern Matching Tests

TEST(AdvancedConditionsTest, LikePattern) {
    auto query = select_from<Users>()
        | where(like("name"_col, "%John%"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE '%John%'");
}

TEST(AdvancedConditionsTest, NotLikePattern) {
    auto query = select_from<Users>()
        | where(not_like("email"_col, "%@example.com"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" NOT LIKE '%@example.com'");
}

TEST(AdvancedConditionsTest, LikeWithPrefix) {
    auto query = select_from<Users>()
        | where(like("name"_col, "John%"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE 'John%'");
}

TEST(AdvancedConditionsTest, LikeWithSuffix) {
    auto query = select_from<Users>()
        | where(like("name"_col, "%Smith"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE '%Smith'");
}

TEST(AdvancedConditionsTest, LikeWithSingleChar) {
    auto query = select_from<Users>()
        | where(like("name"_col, "J_hn"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE 'J_hn'");
}

// ILIKE Pattern Matching Tests (PostgreSQL)

TEST(AdvancedConditionsTest, IlikePattern) {
    auto query = select_from<Users>()
        | where(ilike("name"_col, "%john%"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" ILIKE '%john%'");
}

TEST(AdvancedConditionsTest, NotIlikePattern) {
    auto query = select_from<Users>()
        | where(not_ilike("email"_col, "%@EXAMPLE.COM"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" NOT ILIKE '%@EXAMPLE.COM'");
}

// IS NULL / IS NOT NULL Tests

TEST(AdvancedConditionsTest, IsNull) {
    auto query = select_from<Users>()
        | where(is_null("email"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" IS NULL");
}

TEST(AdvancedConditionsTest, IsNotNull) {
    auto query = select_from<Users>()
        | where(is_not_null("email"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" IS NOT NULL");
}

TEST(AdvancedConditionsTest, IsNullMultipleColumns) {
    auto query = select_from<Users>()
        | where(is_null("email"_col) && is_null("city"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" IS NULL AND \"city\" IS NULL");
}

TEST(AdvancedConditionsTest, IsNotNullWithOrderBy) {
    auto query = select_from<Users>()
        | where(is_not_null("age"_col))
        | order_by("age"_col)
        | limit(10_limit);

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"age\" IS NOT NULL ORDER BY \"age\" LIMIT 10");
}

// IN / NOT IN Tests

TEST(AdvancedConditionsTest, InSingleValue) {
    auto query = select_from<Users>()
        | where(in("id"_col, 1));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"id\" IN (1)");
}

TEST(AdvancedConditionsTest, InMultipleIntegers) {
    auto query = select_from<Users>()
        | where(in("id"_col, 1, 2, 3, 4, 5));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"id\" IN (1, 2, 3, 4, 5)");
}

TEST(AdvancedConditionsTest, InMultipleStrings) {
    auto query = select_from<Users>()
        | where(in("city"_col, "New York", "Los Angeles", "Chicago"));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"city\" IN ('New York', 'Los Angeles', 'Chicago')");
}

TEST(AdvancedConditionsTest, NotInMultipleValues) {
    auto query = select_from<Users>()
        | where(not_in("age"_col, 18, 19, 20));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"age\" NOT IN (18, 19, 20)");
}

TEST(AdvancedConditionsTest, NotInStrings) {
    auto query = select_from<Products>()
        | where(not_in("category"_col, "Electronics", "Toys"));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"category\" NOT IN ('Electronics', 'Toys')");
}

// BETWEEN / NOT BETWEEN Tests

TEST(AdvancedConditionsTest, BetweenIntegers) {
    auto query = select_from<Users>()
        | where(between("age"_col, 18, 65));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"age\" BETWEEN 18 AND 65");
}

TEST(AdvancedConditionsTest, BetweenDoubles) {
    auto query = select_from<Products>()
        | where(between("price"_col, 10.0, 100.0));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"price\" BETWEEN 10.000000 AND 100.000000");
}

TEST(AdvancedConditionsTest, NotBetweenIntegers) {
    auto query = select_from<Users>()
        | where(not_between("age"_col, 0, 17));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"age\" NOT BETWEEN 0 AND 17");
}

TEST(AdvancedConditionsTest, NotBetweenDoubles) {
    auto query = select_from<Products>()
        | where(not_between("price"_col, 0.0, 10.0));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"price\" NOT BETWEEN 0.000000 AND 10.000000");
}

// Complex Combinations

TEST(AdvancedConditionsTest, LikeAndIsNotNull) {
    auto query = select_from<Users>()
        | where(like("name"_col, "%John%"_v) && is_not_null("email"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE '%John%' AND \"email\" IS NOT NULL");
}

TEST(AdvancedConditionsTest, InAndBetween) {
    auto query = select_from<Users>()
        | where(in("city"_col, "New York", "Boston") && between("age"_col, 25, 40));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"city\" IN ('New York', 'Boston') AND \"age\" BETWEEN 25 AND 40");
}

TEST(AdvancedConditionsTest, NotLikeOrIsNull) {
    auto query = select_from<Users>()
        | where(not_like("email"_col, "%@spam.com"_v) || is_null("email"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" NOT LIKE '%@spam.com' OR \"email\" IS NULL");
}

TEST(AdvancedConditionsTest, ComplexConditionWithMultipleAdvanced) {
    auto query = select_from<Products>()
        | where(
            between("price"_col, 10.0, 100.0) &&
            not_in("category"_col, "Clearance", "Discontinued") &&
            is_not_null("stock"_col)
        );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"price\" BETWEEN 10.000000 AND 100.000000 AND \"category\" NOT IN ('Clearance', 'Discontinued') AND \"stock\" IS NOT NULL");
}

TEST(AdvancedConditionsTest, NestedConditionsWithAdvanced) {
    auto query = select_from<Users>()
        | where(
            (like("name"_col, "J%"_v) || like("name"_col, "M%"_v)) &&
            (between("age"_col, 20, 40) && is_not_null("city"_col))
        );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE (\"name\" LIKE 'J%' OR \"name\" LIKE 'M%') AND (\"age\" BETWEEN 20 AND 40 AND \"city\" IS NOT NULL)");
}

// Edge Cases

TEST(AdvancedConditionsTest, EmptyPattern) {
    auto query = select_from<Users>()
        | where(like("name"_col, ""_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE ''");
}

TEST(AdvancedConditionsTest, InWithTwoValues) {
    auto query = select_from<Users>()
        | where(in("id"_col, 1, 2));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"id\" IN (1, 2)");
}

TEST(AdvancedConditionsTest, NotInWithSingleValue) {
    auto query = select_from<Users>()
        | where(not_in("id"_col, 999));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"id\" NOT IN (999)");
}

TEST(AdvancedConditionsTest, BetweenSameValues) {
    auto query = select_from<Users>()
        | where(between("age"_col, 25, 25));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"age\" BETWEEN 25 AND 25");
}

// Integration with ORDER BY and LIMIT

TEST(AdvancedConditionsTest, AdvancedConditionsWithOrderByLimit) {
    auto query = select_from<Products>()
        | where(
            between("price"_col, 50.0, 200.0) &&
            in("category"_col, "Electronics", "Computers")
        )
        | order_by("price"_col)
        | limit(20_limit);

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"price\" BETWEEN 50.000000 AND 200.000000 AND \"category\" IN ('Electronics', 'Computers') ORDER BY \"price\" LIMIT 20");
}

TEST(AdvancedConditionsTest, IsNullWithDescendingOrder) {
    auto query = select_from<Users>()
        | where(is_null("city"_col))
        | order_by(desc("name"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"city\" IS NULL ORDER BY \"name\" DESC");
}

// Pattern Matching Special Characters

TEST(AdvancedConditionsTest, LikeWithEscapableChars) {
    auto query = select_from<Users>()
        | where(like("email"_col, "%'test'%"_v));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" LIKE '%''test''%'");
}

TEST(AdvancedConditionsTest, MultiplePatternConditions) {
    auto query = select_from<Users>()
        | where(
            like("name"_col, "John%"_v) &&
            not_like("email"_col, "%@test.com"_v) &&
            ilike("city"_col, "%NEW YORK%"_v)
        );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"name\" LIKE 'John%' AND \"email\" NOT LIKE '%@test.com' AND \"city\" ILIKE '%NEW YORK%'");
}

// Realistic Query Scenarios

TEST(AdvancedConditionsTest, UserSearchQuery) {
    auto query = select_from<Users>()
        | where(
            (like("name"_col, "%Smith%"_v) || like("email"_col, "%smith%"_v)) &&
            between("age"_col, 18, 100) &&
            is_not_null("city"_col)
        )
        | order_by("name"_col)
        | limit(50_limit);

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE (\"name\" LIKE '%Smith%' OR \"email\" LIKE '%smith%') AND \"age\" BETWEEN 18 AND 100 AND \"city\" IS NOT NULL ORDER BY \"name\" LIMIT 50");
}

TEST(AdvancedConditionsTest, ProductFilterQuery) {
    auto query = select_from<Products>()
        | where(
            in("category"_col, "Electronics", "Computers", "Gaming") &&
            between("price"_col, 100.0, 1000.0) &&
            not_in("name"_col, "Refurbished", "Used") &&
            is_not_null("stock"_col)
        )
        | order_by(desc("price"_col));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"products\" WHERE \"category\" IN ('Electronics', 'Computers', 'Gaming') AND \"price\" BETWEEN 100.000000 AND 1000.000000 AND \"name\" NOT IN ('Refurbished', 'Used') AND \"stock\" IS NOT NULL ORDER BY \"price\" DESC");
}

TEST(AdvancedConditionsTest, NullableFieldsQuery) {
    auto query = select_from<Users>()
        | where(
            is_not_null("email"_col) &&
            is_not_null("city"_col) &&
            not_in("age"_col, 0) &&
            like("name"_col, "%_%"_v)  // Has at least 2 characters
        );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE \"email\" IS NOT NULL AND \"city\" IS NOT NULL AND \"age\" NOT IN (0) AND \"name\" LIKE '%_%'");
}

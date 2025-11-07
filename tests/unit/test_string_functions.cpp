#include <gtest/gtest.h>
#include <glz_sqlgen/select_from.hpp>
#include <glz_sqlgen/functions.hpp>
#include <glz_sqlgen/where.hpp>
#include <glz_sqlgen/order_by.hpp>
#include <glz_sqlgen/literals.hpp>

namespace {

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

// Test table
struct Users {
    int id;
    std::string first_name;
    std::string last_name;
    std::string email;
    std::string city;
};

} // namespace

// Glaze metadata
template <>
struct glz::meta<Users> {
    using T = Users;
    static constexpr std::string_view name = "users";
    [[maybe_unused]] static constexpr auto value = glz::object(
        "id", &T::id,
        "first_name", &T::first_name,
        "last_name", &T::last_name,
        "email", &T::email,
        "city", &T::city
    );
};

// CONCAT Tests

TEST(StringFunctionsTest, ConcatTwoColumns) {
    auto query = select_from<Users>(concat("first_name"_c, "last_name"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT CONCAT(\"first_name\", \"last_name\") FROM \"users\"");
}

TEST(StringFunctionsTest, ConcatThreeColumns) {
    auto query = select_from<Users>(
        concat("first_name"_c, " ", "last_name"_c)
    );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT CONCAT(\"first_name\", ' ', \"last_name\") FROM \"users\"");
}

TEST(StringFunctionsTest, ConcatMultipleStrings) {
    auto query = select_from<Users>(
        concat("first_name"_c, " ", "last_name"_c, " <", "email"_c, ">")
    );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT CONCAT(\"first_name\", ' ', \"last_name\", ' <', \"email\", '>') FROM \"users\"");
}

TEST(StringFunctionsTest, ConcatInWhere) {
    auto query = select_from<Users>()
        | where(concat("first_name"_c, "last_name"_c) == "JohnDoe");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE CONCAT(\"first_name\", \"last_name\") = 'JohnDoe'");
}

// LENGTH Tests

TEST(StringFunctionsTest, LengthColumn) {
    auto query = select_from<Users>(length("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT LENGTH(\"email\") FROM \"users\"");
}

TEST(StringFunctionsTest, LengthInWhere) {
    auto query = select_from<Users>()
        | where(length("email"_c) > 20);

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE LENGTH(\"email\") > 20");
}

TEST(StringFunctionsTest, LengthOrderBy) {
    auto query = select_from<Users>()
        | order_by(length("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" ORDER BY LENGTH(\"email\")");
}

// LOWER Tests

TEST(StringFunctionsTest, LowerColumn) {
    auto query = select_from<Users>(lower("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT LOWER(\"email\") FROM \"users\"");
}

TEST(StringFunctionsTest, LowerInWhere) {
    auto query = select_from<Users>()
        | where(lower("email"_c) == "test@example.com");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE LOWER(\"email\") = 'test@example.com'");
}

// UPPER Tests

TEST(StringFunctionsTest, UpperColumn) {
    auto query = select_from<Users>(upper("city"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT UPPER(\"city\") FROM \"users\"");
}

TEST(StringFunctionsTest, UpperInWhere) {
    auto query = select_from<Users>()
        | where(upper("city"_c) == "NEW YORK");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE UPPER(\"city\") = 'NEW YORK'");
}

// TRIM Tests

TEST(StringFunctionsTest, TrimColumn) {
    auto query = select_from<Users>(trim("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT TRIM(\"email\") FROM \"users\"");
}

TEST(StringFunctionsTest, TrimInWhere) {
    auto query = select_from<Users>()
        | where(trim("email"_c) == "test@example.com");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE TRIM(\"email\") = 'test@example.com'");
}

// LTRIM Tests

TEST(StringFunctionsTest, LtrimColumn) {
    auto query = select_from<Users>(ltrim("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT LTRIM(\"email\") FROM \"users\"");
}

// RTRIM Tests

TEST(StringFunctionsTest, RtrimColumn) {
    auto query = select_from<Users>(rtrim("email"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT RTRIM(\"email\") FROM \"users\"");
}

// REPLACE Tests

TEST(StringFunctionsTest, ReplaceColumn) {
    auto query = select_from<Users>(
        replace("email"_c, "@example.com", "@test.com")
    );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT REPLACE(\"email\", '@example.com', '@test.com') FROM \"users\"");
}

TEST(StringFunctionsTest, ReplaceInWhere) {
    auto query = select_from<Users>()
        | where(replace("email"_c, "@", "#") == "test#example.com");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE REPLACE(\"email\", '@', '#') = 'test#example.com'");
}

// SUBSTRING Tests

TEST(StringFunctionsTest, SubstringColumn) {
    auto query = select_from<Users>(substring("email"_c, 1, 10));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT SUBSTR(\"email\", 1, 10) FROM \"users\"");
}

TEST(StringFunctionsTest, SubstringInWhere) {
    auto query = select_from<Users>()
        | where(substring("email"_c, 1, 4) == "test");

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE SUBSTR(\"email\", 1, 4) = 'test'");
}

// Combined Tests

TEST(StringFunctionsTest, CombinedStringFunctions) {
    auto query = select_from<Users>(
        lower(trim(concat("first_name"_c, " ", "last_name"_c)))
    );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT LOWER(TRIM(CONCAT(\"first_name\", ' ', \"last_name\"))) FROM \"users\"");
}

TEST(StringFunctionsTest, MultipleStringFunctionsInWhere) {
    auto query = select_from<Users>()
        | where(
            length(trim("email"_c)) > 10 &&
            lower("city"_c) == "new york"
        );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE LENGTH(TRIM(\"email\")) > 10 AND LOWER(\"city\") = 'new york'");
}

TEST(StringFunctionsTest, StringFunctionsWithOrderBy) {
    auto query = select_from<Users>()
        | where(length("email"_c) > 10)
        | order_by(upper("last_name"_c));

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT * FROM \"users\" WHERE LENGTH(\"email\") > 10 ORDER BY UPPER(\"last_name\")");
}

TEST(StringFunctionsTest, NestedStringFunctions) {
    auto query = select_from<Users>(
        upper(substring(trim("email"_c), 1, 5))
    );

    auto sql = query.build();
    EXPECT_EQ(sql, "SELECT UPPER(SUBSTR(TRIM(\"email\"), 1, 5)) FROM \"users\"");
}

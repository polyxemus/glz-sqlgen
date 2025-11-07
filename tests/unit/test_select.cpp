#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <glz_sqlgen/core.hpp>
#include <glz_sqlgen/query_builders.hpp>
#include <glz_sqlgen/query_clauses.hpp>
#include <optional>

using namespace glz_sqlgen::literals;
using namespace glz_sqlgen;

// Test structs
struct Person {
    std::string name;
    int age;
    double height;
};

struct User {
    int id;
    std::string username;
    std::optional<std::string> email;
    bool active;
};

TEST(SelectTest, SelectAll) {
    auto query = select_from<Person>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT") != std::string::npos);
    EXPECT_TRUE(sql.find("\"name\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"age\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"height\"") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"Person\"") != std::string::npos);
}

TEST(SelectTest, SelectSpecificColumns) {
    auto query = select_from<Person>("name"_c, "age"_c);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT \"name\", \"age\"") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"Person\"") != std::string::npos);
}

TEST(SelectTest, SelectWithWhere) {
    auto query = select_from<Person>() | where("age"_c >= 18);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"age\" >= 18") != std::string::npos);
}

TEST(SelectTest, SelectWithComplexWhere) {
    auto query = select_from<Person>("name"_c) | where("age"_c > 21);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT \"name\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"age\" > 21") != std::string::npos);
}

TEST(SelectTest, SelectWithOrderBy) {
    auto query = select_from<Person>() | order_by("age"_c);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY \"age\"") != std::string::npos);
}

TEST(SelectTest, SelectWithOrderByDesc) {
    auto query = select_from<Person>() | order_by("age"_c.desc());
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("ORDER BY \"age\" DESC") != std::string::npos);
}

TEST(SelectTest, SelectWithMultipleOrderBy) {
    auto query = select_from<Person>() | order_by("age"_c, "name"_c);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("ORDER BY \"age\", \"name\"") != std::string::npos);
}

TEST(SelectTest, SelectWithLimit) {
    auto query = select_from<Person>() | limit(10);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("LIMIT 10") != std::string::npos);
}

TEST(SelectTest, SelectWithLimitOffset) {
    auto query = select_from<Person>() | limit(10, 20);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("LIMIT 10") != std::string::npos);
    EXPECT_TRUE(sql.find("OFFSET 20") != std::string::npos);
}

TEST(SelectTest, SelectWithWhereAndOrderBy) {
    auto query = select_from<Person>()
        | where("age"_c >= 18)
        | order_by("name"_c);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("WHERE \"age\" >= 18") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY \"name\"") != std::string::npos);
}

TEST(SelectTest, SelectWithWhereOrderByAndLimit) {
    auto query = select_from<Person>()
        | where("age"_c >= 18)
        | order_by("age"_c.desc())
        | limit(5);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("WHERE \"age\" >= 18") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY \"age\" DESC") != std::string::npos);
    EXPECT_TRUE(sql.find("LIMIT 5") != std::string::npos);
}

TEST(SelectTest, SelectUserTable) {
    auto query = select_from<User>("username"_c, "email"_c)
        | where("active"_c == true);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT \"username\", \"email\"") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"active\" = 1") != std::string::npos);
}

TEST(SelectTest, SelectWithStringCondition) {
    auto query = select_from<User>() | where("username"_c == "admin");
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("WHERE \"username\" = 'admin'") != std::string::npos);
}

TEST(SelectTest, ComplexQuery) {
    auto query = select_from<Person>("name"_c, "age"_c, "height"_c)
        | where(("age"_c >= 18))
        | order_by("age"_c.desc(), "name"_c)
        | limit(10, 5);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SELECT \"name\", \"age\", \"height\"") != std::string::npos);
    EXPECT_TRUE(sql.find("FROM \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE") != std::string::npos);
    EXPECT_TRUE(sql.find("ORDER BY") != std::string::npos);
    EXPECT_TRUE(sql.find("LIMIT 10") != std::string::npos);
    EXPECT_TRUE(sql.find("OFFSET 5") != std::string::npos);
}

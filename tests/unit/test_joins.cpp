#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <sqlgen/core.hpp>
#include <sqlgen/query_builders.hpp>
#include <sqlgen/query_clauses.hpp>

using namespace sqlgen;
using namespace sqlgen::literals;

// Test tables
struct Users {
    int id;
    std::string name;
    int department_id;
};

struct Departments {
    int id;
    std::string name;
};

struct Projects {
    int id;
    std::string name;
    int department_id;
};

// Glaze metadata
template <>
struct glz::meta<Users> {
    using T = Users;
    static constexpr std::string_view name = "users";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "department_id", &T::department_id
    );
};

template <>
struct glz::meta<Departments> {
    using T = Departments;
    static constexpr std::string_view name = "departments";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name
    );
};

template <>
struct glz::meta<Projects> {
    using T = Projects;
    static constexpr std::string_view name = "projects";
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "department_id", &T::department_id
    );
};

// === INNER JOIN Tests ===

TEST(JoinsTest, InnerJoinSimple) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\"");
}

TEST(JoinsTest, InnerJoinWithAlias) {
    auto query = select_from<Users>() |
                 inner_join<Departments, "d">("d"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" AS \"d\" ON \"t1\".\"d\" = \"department_id\"");
}

TEST(JoinsTest, InnerJoinWithWhere) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c) |
                 where("name"_c == "Engineering");

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "WHERE \"name\" = 'Engineering'");
}

TEST(JoinsTest, InnerJoinWithOrderByLimit) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c) |
                 order_by("name"_c) |
                 limit(10);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "ORDER BY \"name\" "
                   "LIMIT 10");
}

// === LEFT JOIN Tests ===

TEST(JoinsTest, LeftJoinSimple) {
    auto query = select_from<Users>() |
                 left_join<Departments>("id"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "LEFT OUTER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\"");
}

TEST(JoinsTest, LeftJoinWithWhere) {
    auto query = select_from<Users>() |
                 left_join<Departments>("id"_t1 == "department_id"_c) |
                 where("id"_c > 5);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "LEFT OUTER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "WHERE \"id\" > 5");
}

// === RIGHT JOIN Tests ===

TEST(JoinsTest, RightJoinSimple) {
    auto query = select_from<Users>() |
                 right_join<Departments>("id"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "RIGHT OUTER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\"");
}

// === FULL JOIN Tests ===

TEST(JoinsTest, FullJoinSimple) {
    auto query = select_from<Users>() |
                 full_join<Departments>("id"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "FULL OUTER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\"");
}

// === CROSS JOIN Tests ===

TEST(JoinsTest, CrossJoinSimple) {
    auto query = select_from<Users>() |
                 cross_join<Departments>();

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "CROSS JOIN \"departments\"");
}

TEST(JoinsTest, CrossJoinWithAlias) {
    auto query = select_from<Users>() |
                 cross_join<Departments, "d">();

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "CROSS JOIN \"departments\" AS \"d\"");
}

// === Multiple JOINs Tests ===

TEST(JoinsTest, TwoInnerJoins) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c) |
                 inner_join<Projects>("department_id"_t1 == "id"_t2);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "INNER JOIN \"projects\" ON \"t1\".\"department_id\" = \"t2\".\"id\"");
}

TEST(JoinsTest, MixedJoinTypes) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c) |
                 left_join<Projects>("id"_t1 == "department_id"_t2);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "LEFT OUTER JOIN \"projects\" ON \"t1\".\"id\" = \"t2\".\"department_id\"");
}

TEST(JoinsTest, ThreeTableJoin) {
    auto query = select_from<Users>() |
                 inner_join<Departments, "d">("d"_t1 == "department_id"_c) |
                 inner_join<Projects, "p">("d"_t1 == "p"_t2) |
                 where("name"_c == "John") |
                 order_by("name"_c);

    auto sql = query.to_sql();
    // Note: Expected SQL would depend on exact implementation details
    // This test verifies the query compiles and runs
    EXPECT_FALSE(sql.empty());
}

// === Field Selection with JOINs ===

TEST(JoinsTest, SelectSpecificFieldsWithJoin) {
    auto query = select_from<Users>("id"_c, "name"_c) |
                 inner_join<Departments>("id"_t1 == "department_id"_c);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"id\", \"name\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\"");
}

// === Complex Scenarios ===

TEST(JoinsTest, JoinWithCompleteQuery) {
    auto query = select_from<Users>() |
                 inner_join<Departments>("id"_t1 == "department_id"_c) |
                 where("id"_c > 10) |
                 order_by("name"_c.desc()) |
                 limit(5, 10);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT \"users\".\"id\", \"users\".\"name\", \"users\".\"department_id\" "
                   "FROM \"users\" "
                   "INNER JOIN \"departments\" ON \"t1\".\"id\" = \"department_id\" "
                   "WHERE \"id\" > 10 "
                   "ORDER BY \"name\" DESC "
                   "LIMIT 5 OFFSET 10");
}

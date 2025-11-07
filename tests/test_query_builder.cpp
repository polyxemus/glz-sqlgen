#include <gtest/gtest.h>
#include "glz_sqlgen/query_builder.hpp"

using namespace glz_sqlgen;

class QueryBuilderTest : public ::testing::Test {
protected:
    QueryBuilder qb;

    void SetUp() override {
        qb.reset();
    }
};

TEST_F(QueryBuilderTest, SelectAll) {
    auto query = qb.select({}).from("users").build();
    EXPECT_EQ(query, "SELECT * FROM users");
}

TEST_F(QueryBuilderTest, SelectSpecificColumns) {
    auto query = qb.select({"id", "name", "email"})
                   .from("users")
                   .build();
    EXPECT_EQ(query, "SELECT id, name, email FROM users");
}

TEST_F(QueryBuilderTest, SelectWithWhere) {
    auto query = qb.select({"id", "name"})
                   .from("users")
                   .where("age > 18")
                   .build();
    EXPECT_EQ(query, "SELECT id, name FROM users WHERE age > 18");
}

TEST_F(QueryBuilderTest, SelectAllWithWhere) {
    auto query = qb.select({})
                   .from("products")
                   .where("price < 100.0")
                   .build();
    EXPECT_EQ(query, "SELECT * FROM products WHERE price < 100.0");
}

TEST_F(QueryBuilderTest, ResetBuilder) {
    qb.select({"id"}).from("users").where("active = 1");
    qb.reset();
    auto query = qb.select({"name"}).from("products").build();
    EXPECT_EQ(query, "SELECT name FROM products");
}

TEST_F(QueryBuilderTest, EmptyQuery) {
    auto query = qb.build();
    EXPECT_EQ(query, "SELECT *");
}

TEST_F(QueryBuilderTest, ChainedCalls) {
    auto query = qb.select({"a", "b", "c"})
                   .from("table1")
                   .where("x = 1")
                   .build();
    EXPECT_EQ(query, "SELECT a, b, c FROM table1 WHERE x = 1");
}

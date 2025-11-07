#include <gtest/gtest.h>
#include <optional>
#include "glz_sqlgen/literals.hpp"
#include "glz_sqlgen/insert.hpp"
#include "glz_sqlgen/update.hpp"
#include "glz_sqlgen/delete_from.hpp"
#include "glz_sqlgen/create_table.hpp"
#include "glz_sqlgen/where.hpp"

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

// INSERT Tests
TEST(InsertTest, BasicInsert) {
    auto query = insert<Person>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("INSERT INTO \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("(\"name\", \"age\", \"height\")") != std::string::npos);
    EXPECT_TRUE(sql.find("VALUES (?, ?, ?)") != std::string::npos);
}

TEST(InsertTest, InsertWithMoreFields) {
    auto query = insert<User>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("INSERT INTO \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("(\"id\", \"username\", \"email\", \"active\")") != std::string::npos);
    EXPECT_TRUE(sql.find("VALUES (?, ?, ?, ?)") != std::string::npos);
}

TEST(InsertTest, InsertOrReplace) {
    auto query = insert_or_replace<Person>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("INSERT OR REPLACE INTO \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("VALUES (?, ?, ?)") != std::string::npos);
}

// UPDATE Tests
TEST(UpdateTest, UpdateSingleColumn) {
    auto query = update<Person>(set("age"_c, 30));
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("UPDATE \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("SET \"age\" = 30") != std::string::npos);
}

TEST(UpdateTest, UpdateMultipleColumns) {
    auto query = update<Person>(
        set("age"_c, 30),
        set("height"_c, 1.75)
    );
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("UPDATE \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("SET") != std::string::npos);
    EXPECT_TRUE(sql.find("\"age\" = 30") != std::string::npos);
    EXPECT_TRUE(sql.find("\"height\" = 1.75") != std::string::npos);
}

TEST(UpdateTest, UpdateWithWhere) {
    auto query = update<Person>(set("age"_c, 31))
        | where("age"_c == 30);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("UPDATE \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("SET \"age\" = 31") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"age\" = 30") != std::string::npos);
}

TEST(UpdateTest, UpdateWithComplexWhere) {
    auto query = update<User>(
        set("active"_c, true)
    ) | where("username"_c == "admin");
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("UPDATE \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("SET \"active\" = 1") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"username\" = 'admin'") != std::string::npos);
}

TEST(UpdateTest, UpdateStringField) {
    auto query = update<User>(set("username"_c, std::string("newname")));
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("SET \"username\" = 'newname'") != std::string::npos);
}

// DELETE Tests
TEST(DeleteTest, DeleteAll) {
    auto query = delete_from<Person>();
    auto sql = query.to_sql();

    EXPECT_EQ(sql, "DELETE FROM \"Person\"");
}

TEST(DeleteTest, DeleteWithWhere) {
    auto query = delete_from<Person>() | where("age"_c < 18);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("DELETE FROM \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"age\" < 18") != std::string::npos);
}

TEST(DeleteTest, DeleteWithComplexWhere) {
    auto query = delete_from<User>() | where("active"_c == false);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("DELETE FROM \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("WHERE \"active\" = 0") != std::string::npos);
}

TEST(DeleteTest, DeleteWithStringCondition) {
    auto query = delete_from<User>() | where("username"_c == "deleted");
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("WHERE \"username\" = 'deleted'") != std::string::npos);
}

// CREATE TABLE Tests
TEST(CreateTableTest, BasicCreateTable) {
    auto query = create_table<Person>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("CREATE TABLE \"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"name\" TEXT NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"age\" INTEGER NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"height\" REAL NOT NULL") != std::string::npos);
}

TEST(CreateTableTest, CreateTableIfNotExists) {
    auto query = create_table<Person>(true);
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("CREATE TABLE IF NOT EXISTS \"Person\"") != std::string::npos);
}

TEST(CreateTableTest, CreateTableWithOptionalFields) {
    auto query = create_table<User>();
    auto sql = query.to_sql();

    EXPECT_TRUE(sql.find("CREATE TABLE \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"id\" INTEGER NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"username\" TEXT NOT NULL") != std::string::npos);
    // email should be nullable
    EXPECT_TRUE(sql.find("\"email\" TEXT") != std::string::npos);
    EXPECT_FALSE(sql.find("\"email\" TEXT NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"active\" INTEGER NOT NULL") != std::string::npos);
}

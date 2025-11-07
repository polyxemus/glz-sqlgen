#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <optional>
#include "sqlgen/transpilation_advanced.hpp"

using namespace sqlgen::transpilation;

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

struct Product {
    int64_t id;
    std::string name;
    double price;
    std::optional<std::string> description;
};

TEST(TableInfoTest, GetTableName) {
    auto name = get_table_name<Person>();
    EXPECT_EQ(name, "Person");

    auto user_name = get_table_name<User>();
    EXPECT_EQ(user_name, "User");
}

TEST(TableInfoTest, GetFields) {
    auto fields = get_fields<Person>();

    ASSERT_EQ(fields.size(), 3);

    EXPECT_EQ(fields[0].name, "name");
    EXPECT_EQ(fields[0].sql_type, "TEXT");
    EXPECT_FALSE(fields[0].nullable);

    EXPECT_EQ(fields[1].name, "age");
    EXPECT_EQ(fields[1].sql_type, "INTEGER");
    EXPECT_FALSE(fields[1].nullable);

    EXPECT_EQ(fields[2].name, "height");
    EXPECT_EQ(fields[2].sql_type, "REAL");
    EXPECT_FALSE(fields[2].nullable);
}

TEST(TableInfoTest, GetFieldsWithOptional) {
    auto fields = get_fields<User>();

    ASSERT_EQ(fields.size(), 4);

    EXPECT_EQ(fields[0].name, "id");
    EXPECT_FALSE(fields[0].nullable);

    EXPECT_EQ(fields[1].name, "username");
    EXPECT_FALSE(fields[1].nullable);

    EXPECT_EQ(fields[2].name, "email");
    EXPECT_TRUE(fields[2].nullable);  // std::optional is nullable

    EXPECT_EQ(fields[3].name, "active");
    EXPECT_FALSE(fields[3].nullable);
}

TEST(TableInfoTest, CreateTableSql) {
    auto sql = create_table_sql<Person>();

    EXPECT_TRUE(sql.find("CREATE TABLE") != std::string::npos);
    EXPECT_TRUE(sql.find("\"Person\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"name\" TEXT NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"age\" INTEGER NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"height\" REAL NOT NULL") != std::string::npos);
}

TEST(TableInfoTest, CreateTableSqlIfNotExists) {
    auto sql = create_table_sql<Person>(true);

    EXPECT_TRUE(sql.find("CREATE TABLE IF NOT EXISTS") != std::string::npos);
    EXPECT_TRUE(sql.find("\"Person\"") != std::string::npos);
}

TEST(TableInfoTest, CreateTableWithNullable) {
    auto sql = create_table_sql<User>();

    EXPECT_TRUE(sql.find("\"id\" INTEGER NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"username\" TEXT NOT NULL") != std::string::npos);
    // email should be nullable (no NOT NULL)
    EXPECT_TRUE(sql.find("\"email\" TEXT") != std::string::npos);
    EXPECT_FALSE(sql.find("\"email\" TEXT NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"active\" INTEGER NOT NULL") != std::string::npos);
}

TEST(TableInfoTest, SelectFieldList) {
    auto sql = select_field_list<Person>();

    EXPECT_EQ(sql, "\"name\", \"age\", \"height\"");
}

TEST(TableInfoTest, SelectFieldListWithTableAlias) {
    auto sql = select_field_list<Person>("t1");

    EXPECT_EQ(sql, "\"t1\".\"name\", \"t1\".\"age\", \"t1\".\"height\"");
}

TEST(TableInfoTest, SelectFieldListWithAliasExplicit) {
    auto sql = select_field_list_with_alias<Person>("p");

    EXPECT_TRUE(sql.find("\"p\".\"name\" AS \"name\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"p\".\"age\" AS \"age\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"p\".\"height\" AS \"height\"") != std::string::npos);
}

TEST(TableInfoTest, InsertFieldList) {
    auto sql = insert_field_list<Person>();

    EXPECT_EQ(sql, "\"name\", \"age\", \"height\"");
}

TEST(TableInfoTest, InsertPlaceholders) {
    auto sql = insert_placeholders<Person>();

    EXPECT_EQ(sql, "?, ?, ?");
}

TEST(TableInfoTest, InsertPlaceholdersMultipleFields) {
    auto sql = insert_placeholders<User>();

    EXPECT_EQ(sql, "?, ?, ?, ?");
}

TEST(TableInfoTest, CompleteInsertStatement) {
    // Simulate building a complete INSERT statement
    std::string sql = "INSERT INTO ";
    sql += "\"";
    sql += get_table_name<Product>();
    sql += "\" (";
    sql += insert_field_list<Product>();
    sql += ") VALUES (";
    sql += insert_placeholders<Product>();
    sql += ")";

    EXPECT_TRUE(sql.find("INSERT INTO \"Product\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"id\", \"name\", \"price\", \"description\"") != std::string::npos);
    EXPECT_TRUE(sql.find("?, ?, ?, ?") != std::string::npos);
}

TEST(TableInfoTest, CompleteSelectStatement) {
    // Simulate building a complete SELECT statement
    std::string sql = "SELECT ";
    sql += select_field_list<Person>();
    sql += " FROM ";
    sql += "\"";
    sql += get_table_name<Person>();
    sql += "\"";

    EXPECT_EQ(sql, "SELECT \"name\", \"age\", \"height\" FROM \"Person\"");
}

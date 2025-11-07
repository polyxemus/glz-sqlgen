#include <gtest/gtest.h>
#include "sqlgen/sqlite.hpp"
#include "sqlgen/query_builders.hpp"
#include "sqlgen/query_clauses.hpp"

namespace sqlgen::test {

// Test table schema
struct User {
    int id;
    std::string name;
    int age;
};

class SQLiteTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Connect to in-memory database
        auto result = sqlite::connect(":memory:");
        ASSERT_TRUE(result.has_value()) << "Failed to connect: " << result.error();
        conn_ = std::move(result.value());
    }

    sqlite::Connection conn_{std::move(sqlite::connect(":memory:").value())};
};

TEST_F(SQLiteTest, ConnectToMemoryDatabase) {
    // Connection already created in SetUp
    SUCCEED();
}

TEST_F(SQLiteTest, ConnectToFileDatabase) {
    auto result = sqlite::connect("/tmp/test_glz_sqlgen.db");
    ASSERT_TRUE(result.has_value()) << "Failed to connect: " << result.error();
}

TEST_F(SQLiteTest, ExecuteCreateTable) {
    auto create_sql = create_table<User>().to_sql();
    auto result = conn_.execute(create_sql);
    ASSERT_TRUE(result.has_value()) << "Failed to create table: " << result.error();
}

TEST_F(SQLiteTest, ExecuteInsert) {
    // Create table first
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());

    // Insert data
    std::string insert_sql = "INSERT INTO User (id, name, age) VALUES (1, 'Alice', 30)";
    auto result = conn_.execute(insert_sql);
    ASSERT_TRUE(result.has_value()) << "Failed to insert: " << result.error();
}

TEST_F(SQLiteTest, QueryAndIterateResults) {
    // Create table and insert data
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (1, 'Alice', 30)")).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (2, 'Bob', 25)")).has_value());

    // Query data
    auto result = conn_.query("SELECT * FROM User ORDER BY id");
    ASSERT_TRUE(result.has_value()) << "Failed to query: " << result.error();

    auto iter = std::move(result.value());
    EXPECT_EQ(iter.column_count(), 3);

    // First row
    auto row1 = iter.next();
    ASSERT_TRUE(row1.has_value());
    ASSERT_EQ(row1->size(), 3);
    EXPECT_EQ(row1->at(0).value(), "1");
    EXPECT_EQ(row1->at(1).value(), "Alice");
    EXPECT_EQ(row1->at(2).value(), "30");

    // Second row
    auto row2 = iter.next();
    ASSERT_TRUE(row2.has_value());
    ASSERT_EQ(row2->size(), 3);
    EXPECT_EQ(row2->at(0).value(), "2");
    EXPECT_EQ(row2->at(1).value(), "Bob");
    EXPECT_EQ(row2->at(2).value(), "25");

    // No more rows
    auto row3 = iter.next();
    EXPECT_FALSE(row3.has_value());
    EXPECT_TRUE(iter.end());
}

TEST_F(SQLiteTest, QueryWithNullValues) {
    // Create table with nullable column
    ASSERT_TRUE(conn_.execute(std::string("CREATE TABLE test (id INTEGER, value TEXT)")).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO test VALUES (1, 'hello')")).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO test VALUES (2, NULL)")).has_value());

    auto result = conn_.query("SELECT * FROM test ORDER BY id");
    ASSERT_TRUE(result.has_value());

    auto iter = std::move(result.value());

    // First row - has value
    auto row1 = iter.next();
    ASSERT_TRUE(row1.has_value());
    EXPECT_TRUE(row1->at(1).has_value());
    EXPECT_EQ(row1->at(1).value(), "hello");

    // Second row - NULL value
    auto row2 = iter.next();
    ASSERT_TRUE(row2.has_value());
    EXPECT_FALSE(row2->at(1).has_value()) << "Expected NULL value";
}

TEST_F(SQLiteTest, Transactions) {
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());

    // Begin transaction
    ASSERT_TRUE(conn_.begin_transaction().has_value());

    // Insert data
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (1, 'Alice', 30)")).has_value());

    // Commit
    ASSERT_TRUE(conn_.commit().has_value());

    // Verify data was committed
    auto result = conn_.query("SELECT COUNT(*) FROM User");
    ASSERT_TRUE(result.has_value());
    auto iter = std::move(result.value());
    auto row = iter.next();
    ASSERT_TRUE(row.has_value());
    EXPECT_EQ(row->at(0).value(), "1");
}

TEST_F(SQLiteTest, TransactionRollback) {
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());

    // Begin transaction
    ASSERT_TRUE(conn_.begin_transaction().has_value());

    // Insert data
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (1, 'Alice', 30)")).has_value());

    // Rollback
    ASSERT_TRUE(conn_.rollback().has_value());

    // Verify data was NOT committed
    auto result = conn_.query("SELECT COUNT(*) FROM User");
    ASSERT_TRUE(result.has_value());
    auto iter = std::move(result.value());
    auto row = iter.next();
    ASSERT_TRUE(row.has_value());
    EXPECT_EQ(row->at(0).value(), "0");
}

TEST_F(SQLiteTest, ExecuteQueryBuilder) {
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());

    // Use INSERT query builder
    auto insert_query = insert<User>();
    auto sql = insert_query.to_sql();

    EXPECT_TRUE(sql.find("INSERT INTO \"User\"") != std::string::npos);
    EXPECT_TRUE(sql.find("VALUES") != std::string::npos);
}

TEST_F(SQLiteTest, QueryBuilderIntegration) {
    ASSERT_TRUE(conn_.execute(create_table<User>().to_sql()).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (1, 'Alice', 30)")).has_value());
    ASSERT_TRUE(conn_.execute(std::string("INSERT INTO User (id, name, age) VALUES (2, 'Bob', 25)")).has_value());

    // Use SELECT query builder
    auto select = select_from<User>();
    auto sql = select.to_sql();

    auto result = conn_.query(sql);
    ASSERT_TRUE(result.has_value()) << "Failed to query: " << result.error();

    auto iter = std::move(result.value());
    auto row = iter.next();
    ASSERT_TRUE(row.has_value());
    // First row should be Alice or Bob
    EXPECT_EQ(row->size(), 3);
}

TEST_F(SQLiteTest, ErrorHandling_InvalidSQL) {
    auto result = conn_.execute(std::string("INVALID SQL STATEMENT"));
    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().empty());
}

TEST_F(SQLiteTest, ErrorHandling_QueryInvalidTable) {
    auto result = conn_.query("SELECT * FROM nonexistent_table");
    ASSERT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().empty());
}

TEST_F(SQLiteTest, MoveSemantics) {
    // Create connection
    auto result1 = sqlite::connect(":memory:");
    ASSERT_TRUE(result1.has_value());

    // Move connection
    sqlite::Connection conn2 = std::move(result1.value());

    // Use moved connection
    ASSERT_TRUE(conn2.execute(std::string("CREATE TABLE test (id INTEGER)")).has_value());
}

} // namespace sqlgen::test

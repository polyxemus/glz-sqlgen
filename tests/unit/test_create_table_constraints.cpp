#include <gtest/gtest.h>
#include <sqlgen/constraints.hpp>
#include "sqlgen/transpilation_advanced.hpp"

using namespace sqlgen;
using namespace sqlgen::transpilation;

// ============================================================================
// Test Structs
// ============================================================================

struct BasicUser {
    int id;
    std::string name;
};

struct UserWithPrimaryKey {
    PrimaryKey<int> id;
    std::string name;
};

struct UserWithAutoPK {
    PrimaryKey<int, true> id;
    std::string name;
    std::string email;
};

struct UserWithUnique {
    PrimaryKey<int, true> id;
    Unique<std::string> username;
    std::string email;
};

struct UserWithMultipleConstraints {
    PrimaryKey<int, true> id;
    Unique<std::string> username;
    NotNull<std::string> email;
    std::optional<std::string> bio;
};

struct UserWithStringPK {
    PrimaryKey<std::string> uuid;  // String primary key (no auto-increment)
    std::string name;
};

struct UserWithMultipleUnique {
    PrimaryKey<int, true> id;
    Unique<std::string> username;
    Unique<std::string> email;
};

// ============================================================================
// Field Metadata Tests
// ============================================================================

TEST(CreateTableConstraintsTest, BasicUserNoConstraints) {
    auto fields = get_fields<BasicUser>();

    ASSERT_EQ(fields.size(), 2);
    EXPECT_EQ(fields[0].name, "id");
    EXPECT_FALSE(fields[0].constraints.is_primary_key);
    EXPECT_FALSE(fields[0].constraints.is_unique);
}

TEST(CreateTableConstraintsTest, PrimaryKeyDetection) {
    auto fields = get_fields<UserWithPrimaryKey>();

    ASSERT_EQ(fields.size(), 2);
    EXPECT_EQ(fields[0].name, "id");
    EXPECT_TRUE(fields[0].constraints.is_primary_key);
    EXPECT_FALSE(fields[0].constraints.auto_increment);
    EXPECT_TRUE(fields[0].constraints.is_not_null);
    EXPECT_FALSE(fields[0].nullable);
}

TEST(CreateTableConstraintsTest, AutoIncrementDetection) {
    auto fields = get_fields<UserWithAutoPK>();

    ASSERT_EQ(fields.size(), 3);
    EXPECT_EQ(fields[0].name, "id");
    EXPECT_TRUE(fields[0].constraints.is_primary_key);
    EXPECT_TRUE(fields[0].constraints.auto_increment);
}

TEST(CreateTableConstraintsTest, UniqueDetection) {
    auto fields = get_fields<UserWithUnique>();

    ASSERT_EQ(fields.size(), 3);
    EXPECT_EQ(fields[1].name, "username");
    EXPECT_TRUE(fields[1].constraints.is_unique);
    EXPECT_FALSE(fields[1].constraints.is_primary_key);
}

TEST(CreateTableConstraintsTest, NotNullDetection) {
    auto fields = get_fields<UserWithMultipleConstraints>();

    ASSERT_EQ(fields.size(), 4);

    // email field with NotNull
    EXPECT_EQ(fields[2].name, "email");
    EXPECT_TRUE(fields[2].constraints.is_not_null);
    EXPECT_FALSE(fields[2].nullable);

    // bio field is optional (nullable)
    EXPECT_EQ(fields[3].name, "bio");
    EXPECT_TRUE(fields[3].nullable);
    EXPECT_FALSE(fields[3].constraints.is_not_null);
}

// ============================================================================
// CREATE TABLE SQL Generation Tests
// ============================================================================

TEST(CreateTableConstraintsTest, PrimaryKeySQL) {
    auto sql = create_table_sql<UserWithPrimaryKey>();

    // Check for PRIMARY KEY in SQL
    EXPECT_TRUE(sql.find("PRIMARY KEY") != std::string::npos);
    EXPECT_TRUE(sql.find("AUTOINCREMENT") == std::string::npos);  // Not auto-increment
}

TEST(CreateTableConstraintsTest, AutoIncrementSQL) {
    auto sql = create_table_sql<UserWithAutoPK>();

    // Check for PRIMARY KEY AUTOINCREMENT
    EXPECT_TRUE(sql.find("PRIMARY KEY") != std::string::npos);
    EXPECT_TRUE(sql.find("AUTOINCREMENT") != std::string::npos);
}

TEST(CreateTableConstraintsTest, UniqueSQL) {
    auto sql = create_table_sql<UserWithUnique>();

    // Check for UNIQUE constraint
    EXPECT_TRUE(sql.find("UNIQUE") != std::string::npos);
}

TEST(CreateTableConstraintsTest, MultipleConstraintsSQL) {
    auto sql = create_table_sql<UserWithMultipleConstraints>();

    // Should have PRIMARY KEY, UNIQUE, and NOT NULL
    EXPECT_TRUE(sql.find("PRIMARY KEY") != std::string::npos);
    EXPECT_TRUE(sql.find("UNIQUE") != std::string::npos);
    EXPECT_TRUE(sql.find("NOT NULL") != std::string::npos);

    // bio should be nullable (no NOT NULL after it)
    size_t bio_pos = sql.find("\"bio\"");
    ASSERT_NE(bio_pos, std::string::npos);
    // Find the next newline or closing parenthesis after "bio"
    size_t end_pos = sql.find_first_of(",\n)", bio_pos);
    std::string bio_line = sql.substr(bio_pos, end_pos - bio_pos);
    EXPECT_TRUE(bio_line.find("NOT NULL") == std::string::npos);
}

TEST(CreateTableConstraintsTest, CompleteTableSQL) {
    auto sql = create_table_sql<UserWithAutoPK>(true);

    // Verify complete structure
    EXPECT_TRUE(sql.find("CREATE TABLE IF NOT EXISTS") != std::string::npos);
    EXPECT_TRUE(sql.find("\"UserWithAutoPK\"") != std::string::npos);
    EXPECT_TRUE(sql.find("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT") != std::string::npos);
    EXPECT_TRUE(sql.find("\"name\" TEXT NOT NULL") != std::string::npos);
    EXPECT_TRUE(sql.find("\"email\" TEXT NOT NULL") != std::string::npos);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST(CreateTableConstraintsTest, PrimaryKeyWithoutAutoIncrement) {
    auto sql = create_table_sql<UserWithStringPK>();

    EXPECT_TRUE(sql.find("PRIMARY KEY") != std::string::npos);
    EXPECT_TRUE(sql.find("AUTOINCREMENT") == std::string::npos);
}

TEST(CreateTableConstraintsTest, MultipleUniqueFields) {
    auto sql = create_table_sql<UserWithMultipleUnique>();

    // Count occurrences of UNIQUE
    size_t count = 0;
    size_t pos = 0;
    std::string search = "UNIQUE";
    while ((pos = sql.find(search, pos)) != std::string::npos) {
        ++count;
        pos += search.length();
    }

    EXPECT_EQ(count, 2);  // Two UNIQUE constraints
}

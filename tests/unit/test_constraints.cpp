#include <gtest/gtest.h>
#include <sqlgen/constraints.hpp>

using namespace sqlgen;

// ============================================================================
// PrimaryKey Tests
// ============================================================================

TEST(ConstraintsTest, PrimaryKeyBasic) {
    PrimaryKey<int> pk(42);
    EXPECT_EQ(pk.get(), 42);
    EXPECT_EQ(pk(), 42);
    EXPECT_TRUE(PrimaryKey<int>::is_primary_key);
    EXPECT_FALSE(PrimaryKey<int>::is_auto_increment);
}

TEST(ConstraintsTest, PrimaryKeyAutoIncrement) {
    PrimaryKey<int, true> pk;
    (void)pk; // Suppress unused variable warning
    EXPECT_TRUE((PrimaryKey<int, true>::is_auto_increment));
    EXPECT_TRUE((PrimaryKey<int, true>::is_primary_key));
}

TEST(ConstraintsTest, PrimaryKeyAssignment) {
    PrimaryKey<int> pk;
    pk = 100;
    EXPECT_EQ(pk.get(), 100);

    pk.get() = 200;
    EXPECT_EQ(pk(), 200);
}

TEST(ConstraintsTest, PrimaryKeyConversion) {
    PrimaryKey<int> pk(42);
    int value = pk;  // Implicit conversion
    EXPECT_EQ(value, 42);
}

TEST(ConstraintsTest, PrimaryKeyComparison) {
    PrimaryKey<int> pk1(10);
    PrimaryKey<int> pk2(20);
    PrimaryKey<int> pk3(10);

    EXPECT_TRUE(pk1 == pk3);
    EXPECT_TRUE(pk1 != pk2);
    EXPECT_TRUE(pk1 < pk2);
    EXPECT_TRUE(pk2 > pk1);
    EXPECT_TRUE(pk1 <= pk3);
    EXPECT_TRUE(pk1 >= pk3);
}

TEST(ConstraintsTest, PrimaryKeyInt64) {
    PrimaryKey<int64_t, true> pk(9223372036854775807LL);
    EXPECT_EQ(pk.get(), 9223372036854775807LL);
}

TEST(ConstraintsTest, PrimaryKeyString) {
    PrimaryKey<std::string> pk("user123");
    EXPECT_EQ(pk.get(), "user123");
    EXPECT_FALSE(PrimaryKey<std::string>::is_auto_increment);
}

// ============================================================================
// Unique Tests
// ============================================================================

TEST(ConstraintsTest, UniqueBasic) {
    Unique<std::string> username("alice");
    EXPECT_EQ(username.get(), "alice");
    EXPECT_TRUE(Unique<std::string>::is_unique);
}

TEST(ConstraintsTest, UniqueAssignment) {
    Unique<std::string> username;
    username = "bob";
    EXPECT_EQ(username.get(), "bob");
}

TEST(ConstraintsTest, UniqueConversion) {
    Unique<int> value(42);
    int num = value;  // Implicit conversion
    EXPECT_EQ(num, 42);
}

TEST(ConstraintsTest, UniqueComparison) {
    Unique<std::string> u1("alice");
    Unique<std::string> u2("bob");
    Unique<std::string> u3("alice");

    EXPECT_TRUE(u1 == u3);
    EXPECT_TRUE(u1 != u2);
}

// ============================================================================
// NotNull Tests
// ============================================================================

TEST(ConstraintsTest, NotNullBasic) {
    NotNull<std::string> email("user@example.com");
    EXPECT_EQ(email.get(), "user@example.com");
    EXPECT_TRUE(NotNull<std::string>::is_not_null);
}

TEST(ConstraintsTest, NotNullAssignment) {
    NotNull<int> count;
    count = 5;
    EXPECT_EQ(count.get(), 5);
}

TEST(ConstraintsTest, NotNullConversion) {
    NotNull<double> value(3.14);
    double num = value;  // Implicit conversion
    EXPECT_DOUBLE_EQ(num, 3.14);
}

// ============================================================================
// Trait Tests
// ============================================================================

TEST(ConstraintsTraitsTest, DetectPrimaryKey) {
    using PK = PrimaryKey<int>;
    EXPECT_TRUE(constraints::is_primary_key_v<PK>);
    EXPECT_FALSE(constraints::is_primary_key_v<int>);
    EXPECT_FALSE(constraints::is_primary_key_v<std::string>);
}

TEST(ConstraintsTraitsTest, DetectAutoIncrement) {
    using PK_Auto = PrimaryKey<int, true>;
    using PK_NoAuto = PrimaryKey<int, false>;

    EXPECT_TRUE(constraints::is_auto_increment_v<PK_Auto>);
    EXPECT_FALSE(constraints::is_auto_increment_v<PK_NoAuto>);
    EXPECT_FALSE(constraints::is_auto_increment_v<int>);
}

TEST(ConstraintsTraitsTest, DetectUnique) {
    using U = Unique<std::string>;
    EXPECT_TRUE(constraints::is_unique_v<U>);
    EXPECT_FALSE(constraints::is_unique_v<std::string>);
    EXPECT_FALSE(constraints::is_unique_v<int>);
}

TEST(ConstraintsTraitsTest, DetectNotNull) {
    using NN = NotNull<std::string>;
    EXPECT_TRUE(constraints::is_not_null_v<NN>);
    EXPECT_FALSE(constraints::is_not_null_v<std::string>);
}

TEST(ConstraintsTraitsTest, UnderlyingType) {
    using PK = PrimaryKey<int>;
    using U = Unique<std::string>;
    using NN = NotNull<double>;

    static_assert(std::is_same_v<constraints::underlying_type_t<PK>, int>);
    static_assert(std::is_same_v<constraints::underlying_type_t<U>, std::string>);
    static_assert(std::is_same_v<constraints::underlying_type_t<NN>, double>);
    static_assert(std::is_same_v<constraints::underlying_type_t<int>, int>);
}

TEST(ConstraintsTraitsTest, ConstraintWrapperDetection) {
    EXPECT_TRUE(constraints::is_constraint_wrapper_v<PrimaryKey<int>>);
    EXPECT_TRUE(constraints::is_constraint_wrapper_v<Unique<std::string>>);
    EXPECT_TRUE(constraints::is_constraint_wrapper_v<NotNull<double>>);
    EXPECT_FALSE(constraints::is_constraint_wrapper_v<int>);
    EXPECT_FALSE(constraints::is_constraint_wrapper_v<std::string>);
}

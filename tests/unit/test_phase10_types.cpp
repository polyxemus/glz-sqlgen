#include <gtest/gtest.h>
#include <sqlgen/constraints.hpp>
#include <stdexcept>

using namespace sqlgen;

// ============================================================================
// Foreign Key Tests
// ============================================================================

struct ReferencedTable {
    int id;
};

TEST(Phase10TypesTest, ForeignKeyBasic) {
    ForeignKey<int, ReferencedTable, "id"> fk(42);
    EXPECT_EQ(fk.get(), 42);
    EXPECT_TRUE((ForeignKey<int, ReferencedTable, "id">::is_foreign_key));
}

TEST(Phase10TypesTest, ForeignKeyReferentialActions) {
    using FK_Cascade = ForeignKey<int, ReferencedTable, "id",
                                  ReferentialAction::CASCADE,
                                  ReferentialAction::CASCADE>;
    EXPECT_EQ(FK_Cascade::on_delete, ReferentialAction::CASCADE);
    EXPECT_EQ(FK_Cascade::on_update, ReferentialAction::CASCADE);

    EXPECT_EQ(referential_action_to_sql(ReferentialAction::CASCADE), "CASCADE");
    EXPECT_EQ(referential_action_to_sql(ReferentialAction::SET_NULL), "SET NULL");
    EXPECT_EQ(referential_action_to_sql(ReferentialAction::RESTRICT), "RESTRICT");
}

// ============================================================================
// Varchar Tests
// ============================================================================

TEST(Phase10TypesTest, VarcharBasic) {
    Varchar<50> name("Alice");
    EXPECT_EQ(name.get(), "Alice");
    EXPECT_EQ(Varchar<50>::max_length, 50);
}

TEST(Phase10TypesTest, VarcharLengthValidation) {
    Varchar<10> short_str;
    EXPECT_NO_THROW(short_str = "123456789");  // 9 chars - OK
    EXPECT_NO_THROW(short_str = "1234567890"); // 10 chars - OK
    EXPECT_THROW(short_str = "12345678901", std::length_error);  // 11 chars - fails
}

TEST(Phase10TypesTest, VarcharConversion) {
    Varchar<20> name("Bob");
    std::string str = name;  // Implicit conversion
    EXPECT_EQ(str, "Bob");
}

TEST(Phase10TypesTest, VarcharComparison) {
    Varchar<20> v1("alice");
    Varchar<20> v2("bob");
    Varchar<20> v3("alice");

    EXPECT_TRUE(v1 == v3);
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v1 < v2);
}

// ============================================================================
// Char Tests
// ============================================================================

TEST(Phase10TypesTest, CharBasic) {
    Char<10> code("ABC");
    EXPECT_EQ(code.get(), "ABC");  // Trimmed
    EXPECT_EQ(code.get_raw().length(), 10);  // Padded internally
    EXPECT_EQ(Char<10>::fixed_length, 10);
}

TEST(Phase10TypesTest, CharPadding) {
    Char<5> code("Hi");
    EXPECT_EQ(code.get(), "Hi");  // Trimmed value
    EXPECT_EQ(code.get_raw(), "Hi   ");  // Padded with 3 spaces
    EXPECT_EQ(code.size(), 5);
}

TEST(Phase10TypesTest, CharLengthValidation) {
    Char<5> code;
    EXPECT_NO_THROW(code = "12345");  // Exactly 5 chars - OK
    EXPECT_THROW(code = "123456", std::length_error);  // 6 chars - fails
}

// ============================================================================
// JSON Tests
// ============================================================================

struct TestData {
    int id;
    std::string name;
};

template <>
struct glz::meta<TestData> {
    using T = TestData;
    static constexpr auto value = object("id", &T::id, "name", &T::name);
};

TEST(Phase10TypesTest, JSONBasic) {
    JSON<TestData> json_field;
    json_field.get().id = 123;
    json_field.get().name = "test";

    EXPECT_EQ(json_field.get().id, 123);
    EXPECT_EQ(json_field.get().name, "test");
    EXPECT_TRUE(JSON<TestData>::is_json);
}

TEST(Phase10TypesTest, JSONSerialization) {
    JSON<TestData> json_field;
    json_field.get().id = 42;
    json_field.get().name = "Alice";

    std::string json_str = json_field.to_json();
    EXPECT_FALSE(json_str.empty());

    // Parse back
    JSON<TestData> json_field2;
    EXPECT_NO_THROW(json_field2.from_json(json_str));
    EXPECT_EQ(json_field2.get().id, 42);
    EXPECT_EQ(json_field2.get().name, "Alice");
}

// ============================================================================
// Date Tests
// ============================================================================

TEST(Phase10TypesTest, DateBasic) {
    Date date(2024, 11, 7);
    EXPECT_EQ(date.year, 2024);
    EXPECT_EQ(date.month, 11);
    EXPECT_EQ(date.day, 7);
    EXPECT_TRUE(Date::is_date);
}

TEST(Phase10TypesTest, DateToString) {
    Date date(2024, 11, 7);
    EXPECT_EQ(date.to_string(), "2024-11-07");
}

TEST(Phase10TypesTest, DateFromString) {
    Date date("2024-11-07");
    EXPECT_EQ(date.year, 2024);
    EXPECT_EQ(date.month, 11);
    EXPECT_EQ(date.day, 7);
}

TEST(Phase10TypesTest, DateValidation) {
    EXPECT_THROW(Date(2024, 13, 1), std::invalid_argument);  // Invalid month
    EXPECT_THROW(Date(2024, 1, 32), std::invalid_argument);  // Invalid day
    EXPECT_THROW(Date("invalid"), std::invalid_argument);
}

TEST(Phase10TypesTest, DateComparison) {
    Date d1(2024, 11, 7);
    Date d2(2024, 11, 8);
    Date d3(2024, 11, 7);

    EXPECT_TRUE(d1 == d3);
    EXPECT_TRUE(d1 != d2);
    EXPECT_TRUE(d1 < d2);
}

// ============================================================================
// DateTime Tests
// ============================================================================

TEST(Phase10TypesTest, DateTimeBasic) {
    DateTime dt(2024, 11, 7, 14, 30, 45);
    EXPECT_EQ(dt.year, 2024);
    EXPECT_EQ(dt.month, 11);
    EXPECT_EQ(dt.day, 7);
    EXPECT_EQ(dt.hour, 14);
    EXPECT_EQ(dt.minute, 30);
    EXPECT_EQ(dt.second, 45);
    EXPECT_TRUE(DateTime::is_datetime);
}

TEST(Phase10TypesTest, DateTimeToString) {
    DateTime dt(2024, 11, 7, 14, 30, 45);
    EXPECT_EQ(dt.to_string(), "2024-11-07 14:30:45");
}

TEST(Phase10TypesTest, DateTimeFromString) {
    DateTime dt1("2024-11-07 14:30:45");
    EXPECT_EQ(dt1.year, 2024);
    EXPECT_EQ(dt1.hour, 14);

    DateTime dt2("2024-11-07T14:30:45");  // ISO 8601 format
    EXPECT_EQ(dt2.year, 2024);
    EXPECT_EQ(dt2.hour, 14);
}

TEST(Phase10TypesTest, DateTimeValidation) {
    EXPECT_THROW(DateTime(2024, 11, 7, 24, 0, 0), std::invalid_argument);  // Invalid hour
    EXPECT_THROW(DateTime(2024, 11, 7, 14, 60, 0), std::invalid_argument);  // Invalid minute
}

// ============================================================================
// Timestamp Tests
// ============================================================================

TEST(Phase10TypesTest, TimestampBasic) {
    Timestamp ts(1234567890);
    EXPECT_EQ(ts.get(), 1234567890);
    EXPECT_TRUE(Timestamp::is_timestamp);
}

TEST(Phase10TypesTest, TimestampNow) {
    Timestamp ts = Timestamp::now();
    EXPECT_GT(ts.get(), 0);
}

TEST(Phase10TypesTest, TimestampToDateTime) {
    Timestamp ts(0);  // Unix epoch
    DateTime dt = ts.to_datetime();
    EXPECT_EQ(dt.year, 1970);
    EXPECT_EQ(dt.month, 1);
    EXPECT_EQ(dt.day, 1);
}

// ============================================================================
// UUID Tests
// ============================================================================

TEST(Phase10TypesTest, UUIDBasic) {
    UUID uuid("550e8400-e29b-41d4-a716-446655440000");
    EXPECT_EQ(uuid.get(), "550e8400-e29b-41d4-a716-446655440000");
    EXPECT_TRUE(UUID::is_uuid);
}

TEST(Phase10TypesTest, UUIDValidation) {
    EXPECT_NO_THROW(UUID("550e8400-e29b-41d4-a716-446655440000"));  // Valid
    EXPECT_THROW(UUID("invalid"), std::invalid_argument);  // Too short
    EXPECT_THROW(UUID("550e8400-e29b-41d4-a716-44665544000"), std::invalid_argument);  // Wrong length
    EXPECT_THROW(UUID("550e8400xe29bx41d4xa716x446655440000"), std::invalid_argument);  // Wrong format
    EXPECT_THROW(UUID("550e8400-e29b-41d4-a716-44665544000g"), std::invalid_argument);  // Non-hex char
}

// ============================================================================
// Validated Types Tests
// ============================================================================

TEST(Phase10TypesTest, AlphaNumericBasic) {
    AlphaNumeric an("user_123");
    EXPECT_EQ(an.get(), "user_123");
}

TEST(Phase10TypesTest, AlphaNumericValidation) {
    AlphaNumeric an;
    EXPECT_NO_THROW(an = "ValidName_123");
    EXPECT_THROW(an = "invalid-name", std::invalid_argument);  // Dash not allowed
    EXPECT_THROW(an = "invalid name", std::invalid_argument);  // Space not allowed
    EXPECT_THROW(an = "", std::invalid_argument);  // Empty not allowed
}

TEST(Phase10TypesTest, Base64EncodedBasic) {
    Base64Encoded b64("SGVsbG8gV29ybGQ=");
    EXPECT_EQ(b64.get(), "SGVsbG8gV29ybGQ=");
}

TEST(Phase10TypesTest, Base64EncodedValidation) {
    Base64Encoded b64;
    EXPECT_NO_THROW(b64 = "SGVsbG8=");  // Valid
    EXPECT_THROW(b64 = "Invalid@Char", std::invalid_argument);  // @ not allowed
    EXPECT_THROW(b64 = "Abc", std::invalid_argument);  // Not multiple of 4
}

TEST(Phase10TypesTest, EmailBasic) {
    Email email("user@example.com");
    EXPECT_EQ(email.get(), "user@example.com");
}

TEST(Phase10TypesTest, EmailValidation) {
    Email email;
    EXPECT_NO_THROW(email = "user@example.com");
    EXPECT_NO_THROW(email = "user.name+tag@example.co.uk");
    EXPECT_THROW(email = "invalid", std::invalid_argument);
    EXPECT_THROW(email = "@example.com", std::invalid_argument);
    EXPECT_THROW(email = "user@", std::invalid_argument);
    EXPECT_THROW(email = "", std::invalid_argument);
}

TEST(Phase10TypesTest, UUIDv4Basic) {
    UUIDv4 uuid("550e8400-e29b-41d4-a716-446655440000");
    EXPECT_EQ(uuid.get(), "550e8400-e29b-41d4-a716-446655440000");
}

TEST(Phase10TypesTest, UUIDv4Validation) {
    // Valid UUIDv4 (version 4, variant 8/9/a/b)
    EXPECT_NO_THROW(UUIDv4("550e8400-e29b-41d4-8716-446655440000"));
    EXPECT_NO_THROW(UUIDv4("550e8400-e29b-41d4-9716-446655440000"));
    EXPECT_NO_THROW(UUIDv4("550e8400-e29b-41d4-a716-446655440000"));
    EXPECT_NO_THROW(UUIDv4("550e8400-e29b-41d4-b716-446655440000"));

    // Invalid version (not 4)
    EXPECT_THROW(UUIDv4("550e8400-e29b-31d4-a716-446655440000"), std::invalid_argument);

    // Invalid variant (not 8/9/a/b)
    EXPECT_THROW(UUIDv4("550e8400-e29b-41d4-0716-446655440000"), std::invalid_argument);
}

TEST(Phase10TypesTest, UrlSafeBasic) {
    UrlSafe url("my-file_name.txt");
    EXPECT_EQ(url.get(), "my-file_name.txt");
}

TEST(Phase10TypesTest, UrlSafeValidation) {
    UrlSafe url;
    EXPECT_NO_THROW(url = "valid-name_123.txt");
    EXPECT_NO_THROW(url = "a~b-c_d.e");
    EXPECT_THROW(url = "invalid space", std::invalid_argument);
    EXPECT_THROW(url = "invalid@char", std::invalid_argument);
}

TEST(Phase10TypesTest, HexBasic) {
    Hex hex("deadbeef");
    EXPECT_EQ(hex.get(), "deadbeef");
}

TEST(Phase10TypesTest, HexValidation) {
    Hex hex;
    EXPECT_NO_THROW(hex = "0123456789abcdef");
    EXPECT_NO_THROW(hex = "ABCDEF");
    EXPECT_THROW(hex = "invalid", std::invalid_argument);  // 'i', 'n', 'v', 'l' not hex
    EXPECT_THROW(hex = "", std::invalid_argument);  // Empty not allowed
}

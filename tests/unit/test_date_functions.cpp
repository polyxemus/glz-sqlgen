#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <glz_sqlgen/select_from.hpp>
#include <glz_sqlgen/functions.hpp>
#include <glz_sqlgen/where.hpp>
#include <glz_sqlgen/order_by.hpp>
#include <glz_sqlgen/literals.hpp>

namespace {

using namespace glz_sqlgen;
using namespace glz_sqlgen::literals;

// Test tables
struct Events {
    int id;
    std::string name;
    std::string event_date;
    std::string created_at;
};

struct Timestamps {
    int id;
    std::string datetime_field;
    std::string date_field;
    std::string time_field;
};

} // namespace

// Glaze metadata
template <>
struct glz::meta<Events> {
    using T = Events;
    static constexpr std::string_view name = "events";
    [[maybe_unused]] static constexpr auto value = glz::object(
        "id", &T::id,
        "name", &T::name,
        "event_date", &T::event_date,
        "created_at", &T::created_at
    );
};

template <>
struct glz::meta<Timestamps> {
    using T = Timestamps;
    static constexpr std::string_view name = "timestamps";
    [[maybe_unused]] static constexpr auto value = glz::object(
        "id", &T::id,
        "datetime_field", &T::datetime_field,
        "date_field", &T::date_field,
        "time_field", &T::time_field
    );
};

// YEAR Tests

TEST(DateFunctionsTest, YearColumn) {
    auto query = select_from<Events>(year("event_date"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%Y', \"event_date\") AS INTEGER) FROM \"events\"");
}

TEST(DateFunctionsTest, YearInWhere) {
    auto query = select_from<Events>()
        | where(year("event_date"_c) == 2024);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%Y', \"event_date\") AS INTEGER) = 2024");
}

TEST(DateFunctionsTest, YearWithOrderBy) {
    auto query = select_from<Events>()
        | order_by(year("event_date"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" ORDER BY CAST(strftime('%Y', \"event_date\") AS INTEGER)");
}

// MONTH Tests

TEST(DateFunctionsTest, MonthColumn) {
    auto query = select_from<Events>(month("event_date"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%m', \"event_date\") AS INTEGER) FROM \"events\"");
}

TEST(DateFunctionsTest, MonthInWhere) {
    auto query = select_from<Events>()
        | where(month("event_date"_c) == 12);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%m', \"event_date\") AS INTEGER) = 12");
}

TEST(DateFunctionsTest, MonthRange) {
    auto query = select_from<Events>()
        | where(month("event_date"_c) >= 6 && month("event_date"_c) <= 8);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%m', \"event_date\") AS INTEGER) >= 6 AND CAST(strftime('%m', \"event_date\") AS INTEGER) <= 8");
}

// DAY Tests

TEST(DateFunctionsTest, DayColumn) {
    auto query = select_from<Events>(day("event_date"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%d', \"event_date\") AS INTEGER) FROM \"events\"");
}

TEST(DateFunctionsTest, DayInWhere) {
    auto query = select_from<Events>()
        | where(day("event_date"_c) == 15);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%d', \"event_date\") AS INTEGER) = 15");
}

// HOUR Tests

TEST(DateFunctionsTest, HourColumn) {
    auto query = select_from<Timestamps>(hour("datetime_field"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%H', \"datetime_field\") AS INTEGER) FROM \"timestamps\"");
}

TEST(DateFunctionsTest, HourInWhere) {
    auto query = select_from<Timestamps>()
        | where(hour("datetime_field"_c) >= 9 && hour("datetime_field"_c) <= 17);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"timestamps\" WHERE CAST(strftime('%H', \"datetime_field\") AS INTEGER) >= 9 AND CAST(strftime('%H', \"datetime_field\") AS INTEGER) <= 17");
}

// MINUTE Tests

TEST(DateFunctionsTest, MinuteColumn) {
    auto query = select_from<Timestamps>(minute("datetime_field"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%M', \"datetime_field\") AS INTEGER) FROM \"timestamps\"");
}

TEST(DateFunctionsTest, MinuteInWhere) {
    auto query = select_from<Timestamps>()
        | where(minute("datetime_field"_c) == 30);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"timestamps\" WHERE CAST(strftime('%M', \"datetime_field\") AS INTEGER) = 30");
}

// SECOND Tests

TEST(DateFunctionsTest, SecondColumn) {
    auto query = select_from<Timestamps>(second("datetime_field"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%S', \"datetime_field\") AS INTEGER) FROM \"timestamps\"");
}

TEST(DateFunctionsTest, SecondInWhere) {
    auto query = select_from<Timestamps>()
        | where(second("datetime_field"_c) < 30);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"timestamps\" WHERE CAST(strftime('%S', \"datetime_field\") AS INTEGER) < 30");
}

// WEEKDAY Tests

TEST(DateFunctionsTest, WeekdayColumn) {
    auto query = select_from<Events>(weekday("event_date"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%w', \"event_date\") AS INTEGER) FROM \"events\"");
}

TEST(DateFunctionsTest, WeekdayInWhere) {
    auto query = select_from<Events>()
        | where(weekday("event_date"_c) >= 1 && weekday("event_date"_c) <= 5);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%w', \"event_date\") AS INTEGER) >= 1 AND CAST(strftime('%w', \"event_date\") AS INTEGER) <= 5");
}

// DAYS_BETWEEN Tests

TEST(DateFunctionsTest, DaysBetweenColumns) {
    auto query = select_from<Events>(
        days_between("event_date"_c, "created_at"_c)
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT (julianday(\"created_at\") - julianday(\"event_date\")) FROM \"events\"");
}

TEST(DateFunctionsTest, DaysBetweenInWhere) {
    auto query = select_from<Events>()
        | where(days_between("event_date"_c, "created_at"_c) > 30);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE (julianday(\"created_at\") - julianday(\"event_date\")) > 30");
}

TEST(DateFunctionsTest, DaysBetweenWithValue) {
    auto query = select_from<Events>()
        | where(days_between("event_date"_c, "2024-12-31") <= 90);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE (julianday('2024-12-31') - julianday(\"event_date\")) <= 90");
}

// UNIXEPOCH Tests

TEST(DateFunctionsTest, UnixepochColumn) {
    auto query = select_from<Timestamps>(unixepoch("datetime_field"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT unixepoch(\"datetime_field\") FROM \"timestamps\"");
}

TEST(DateFunctionsTest, UnixepochInWhere) {
    auto query = select_from<Timestamps>()
        | where(unixepoch("datetime_field"_c) > 1700000000);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"timestamps\" WHERE unixepoch(\"datetime_field\") > 1700000000");
}

// COMBINED DATE FUNCTIONS

TEST(DateFunctionsTest, MultipleDateExtractors) {
    auto query = select_from<Events>(
        year("event_date"_c),
        month("event_date"_c),
        day("event_date"_c)
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(strftime('%Y', \"event_date\") AS INTEGER), CAST(strftime('%m', \"event_date\") AS INTEGER), CAST(strftime('%d', \"event_date\") AS INTEGER) FROM \"events\"");
}

TEST(DateFunctionsTest, DateFilterComplex) {
    auto query = select_from<Events>()
        | where(
            year("event_date"_c) == 2024 &&
            month("event_date"_c) >= 6 &&
            weekday("event_date"_c) != 0  // Not Sunday
        );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(strftime('%Y', \"event_date\") AS INTEGER) = 2024 AND CAST(strftime('%m', \"event_date\") AS INTEGER) >= 6 AND CAST(strftime('%w', \"event_date\") AS INTEGER) != 0");
}

TEST(DateFunctionsTest, TimeRangeQuery) {
    auto query = select_from<Timestamps>()
        | where(
            hour("datetime_field"_c) >= 9 &&
            hour("datetime_field"_c) < 17 &&
            minute("datetime_field"_c) >= 0
        );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"timestamps\" WHERE CAST(strftime('%H', \"datetime_field\") AS INTEGER) >= 9 AND CAST(strftime('%H', \"datetime_field\") AS INTEGER) < 17 AND CAST(strftime('%M', \"datetime_field\") AS INTEGER) >= 0");
}

// UTILITY FUNCTIONS - CAST

TEST(UtilityFunctionsTest, CastToInteger) {
    auto query = select_from<Events>(cast<int>("id"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(\"id\" AS INTEGER) FROM \"events\"");
}

TEST(UtilityFunctionsTest, CastToReal) {
    auto query = select_from<Events>(cast<double>("id"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(\"id\" AS REAL) FROM \"events\"");
}

TEST(UtilityFunctionsTest, CastToText) {
    auto query = select_from<Events>(cast<std::string>("id"_c));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(\"id\" AS TEXT) FROM \"events\"");
}

TEST(UtilityFunctionsTest, CastInWhere) {
    auto query = select_from<Events>()
        | where(cast<int>("id"_c) > 100);

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE CAST(\"id\" AS INTEGER) > 100");
}

// UTILITY FUNCTIONS - COALESCE

TEST(UtilityFunctionsTest, CoalesceTwoColumns) {
    auto query = select_from<Events>(coalesce("name"_c, "Unknown"));

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COALESCE(\"name\", 'Unknown') FROM \"events\"");
}

TEST(UtilityFunctionsTest, CoalesceMultipleValues) {
    auto query = select_from<Events>(
        coalesce("name"_c, "event_date"_c, "N/A")
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT COALESCE(\"name\", \"event_date\", 'N/A') FROM \"events\"");
}

TEST(UtilityFunctionsTest, CoalesceInWhere) {
    auto query = select_from<Events>()
        | where(coalesce("name"_c, "") != "");

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT * FROM \"events\" WHERE COALESCE(\"name\", '') != ''");
}

TEST(UtilityFunctionsTest, CoalesceWithCast) {
    auto query = select_from<Events>(
        cast<std::string>(coalesce("id"_c, 0))
    );

    auto sql = query.to_sql();
    EXPECT_EQ(sql, "SELECT CAST(COALESCE(\"id\", 0) AS TEXT) FROM \"events\"");
}

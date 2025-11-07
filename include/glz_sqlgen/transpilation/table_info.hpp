#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <glaze/reflection/to_tuple.hpp>
#include <glaze/reflection/get_name.hpp>
#include <glaze/core/meta.hpp>
#include "to_sql_type.hpp"
#include "quote.hpp"

namespace glz_sqlgen::transpilation {

/// Get the table name for a type
/// Uses glz::meta<T>::name if available, otherwise uses type name
template <class T>
constexpr std::string_view get_table_name() {
    using Type = std::remove_cvref_t<T>;

    if constexpr (requires { glz::meta<Type>::name; }) {
        return glz::meta<Type>::name;
    } else {
        // Extract simple type name from __PRETTY_FUNCTION__ or similar
        std::string_view name = __PRETTY_FUNCTION__;

        #if defined(__GNUC__) || defined(__clang__)
        // Format: ... [T = TypeName]
        auto start = name.find("T = ");
        if (start != std::string_view::npos) {
            start += 4;
            auto end = name.find_first_of("];", start);
            name = name.substr(start, end - start);
        }
        #elif defined(_MSC_VER)
        // Format: ... <TypeName>
        auto start = name.find("get_table_name<");
        if (start != std::string_view::npos) {
            start += 15;
            auto end = name.find('>', start);
            name = name.substr(start, end - start);
        }
        #endif

        // Remove namespace qualifiers
        auto pos = name.rfind("::");
        if (pos != std::string_view::npos) {
            name = name.substr(pos + 2);
        }

        // Remove struct/class prefix
        if (name.starts_with("struct ")) {
            name.remove_prefix(7);
        } else if (name.starts_with("class ")) {
            name.remove_prefix(6);
        }

        return name;
    }
}

/// Field information for a struct field
struct FieldInfo {
    std::string name;
    std::string sql_type;
    bool nullable;
};

/// Get field information for a type using glaze reflection
template <class T>
std::vector<FieldInfo> get_fields() {
    using Type = std::remove_cvref_t<T>;
    std::vector<FieldInfo> fields;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            using FieldType = std::remove_cvref_t<decltype(
                glz::get<Is>(glz::to_tie(std::declval<Type&>())))>;

            FieldInfo info;
            info.name = std::string(glz::member_nameof<Is, Type>);
            info.sql_type = std::string(to_sql_type<FieldType>());
            info.nullable = is_optional_v<FieldType>;

            fields.push_back(std::move(info));
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return fields;
}

/// Generate a comma-separated list of field names
template <class T>
std::string get_field_list() {
    auto fields = get_fields<T>();
    std::string result;

    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) result += ", ";
        result += quote_identifier(fields[i].name);
    }

    return result;
}

/// Generate CREATE TABLE statement for a type
template <class T>
std::string create_table_sql(bool if_not_exists = false) {
    std::string sql = "CREATE TABLE ";

    if (if_not_exists) {
        sql += "IF NOT EXISTS ";
    }

    sql += quote_identifier(std::string(get_table_name<T>()));
    sql += " (\n";

    auto fields = get_fields<T>();
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) sql += ",\n";
        sql += "    ";
        sql += quote_identifier(fields[i].name);
        sql += " ";
        sql += fields[i].sql_type;

        if (!fields[i].nullable) {
            sql += " NOT NULL";
        }
    }

    sql += "\n)";
    return sql;
}

} // namespace glz_sqlgen::transpilation

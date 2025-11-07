#pragma once

#include <string>
#include <vector>
#include <glaze/reflection/to_tuple.hpp>
#include <glaze/reflection/get_name.hpp>
#include "quote.hpp"

namespace glz_sqlgen::transpilation {

/// Generate a SELECT field list from a type
template <class T>
std::string select_field_list(std::string_view table_alias = "") {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";

            if (!table_alias.empty()) {
                result += quote_identifier(std::string(table_alias));
                result += ".";
            }

            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate a SELECT field list with explicit aliasing
template <class T>
std::string select_field_list_with_alias(std::string_view table_alias) {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";

            result += quote_identifier(std::string(table_alias));
            result += ".";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            result += " AS ";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));

            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate an INSERT field list (just field names)
template <class T>
std::string insert_field_list() {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        size_t idx = 0;
        ([&] {
            if (idx > 0) result += ", ";
            result += quote_identifier(std::string(glz::member_nameof<Is, Type>));
            ++idx;
        }(), ...);
    }(std::make_index_sequence<field_count>{});

    return result;
}

/// Generate placeholder list for INSERT VALUES
template <class T>
std::string insert_placeholders() {
    using Type = std::remove_cvref_t<T>;
    std::string result;

    constexpr size_t field_count = glz::detail::count_members<Type>;

    for (size_t i = 0; i < field_count; ++i) {
        if (i > 0) result += ", ";
        result += "?";
    }

    return result;
}

} // namespace glz_sqlgen::transpilation

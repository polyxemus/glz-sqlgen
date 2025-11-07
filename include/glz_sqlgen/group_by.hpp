#pragma once

#include <tuple>
#include <type_traits>

namespace glz_sqlgen {

/// GROUP BY clause wrapper
template <class... ColTypes>
struct GroupBy {
    std::tuple<ColTypes...> columns;

    constexpr GroupBy(ColTypes... cols) : columns(std::make_tuple(cols...)) {}
};

/// Create a GROUP BY clause
template <class... ColTypes>
constexpr auto group_by(const ColTypes&... columns) {
    return GroupBy<std::remove_cvref_t<ColTypes>...>{columns...};
}

} // namespace glz_sqlgen

#pragma once

#include <tuple>

namespace glz_sqlgen {

/// Wrapper for ORDER BY clause
template <class... ColTypes>
struct OrderBy {
    std::tuple<ColTypes...> columns;
};

/// Create an ORDER BY clause from columns
template <class... ColTypes>
auto order_by(const ColTypes&... cols) {
    return OrderBy<ColTypes...>{.columns = std::make_tuple(cols...)};
}

} // namespace glz_sqlgen

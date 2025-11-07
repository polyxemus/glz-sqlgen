#pragma once

#include <type_traits>

namespace glz_sqlgen {

/// Wrapper for WHERE clause conditions
template <class ConditionType>
struct Where {
    ConditionType condition;
};

/// Create a WHERE clause from a condition
template <class ConditionType>
inline auto where(const ConditionType& _cond) {
    return Where<std::remove_cvref_t<ConditionType>>{.condition = _cond};
}

} // namespace glz_sqlgen

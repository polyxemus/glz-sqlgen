#pragma once

#include <type_traits>

namespace glz_sqlgen {

/// Wrapper for HAVING clause conditions
template <class ConditionType>
struct Having {
    ConditionType condition;
};

/// Create a HAVING clause from a condition
template <class ConditionType>
inline auto having(const ConditionType& _cond) {
    return Having<std::remove_cvref_t<ConditionType>>{.condition = _cond};
}

} // namespace glz_sqlgen

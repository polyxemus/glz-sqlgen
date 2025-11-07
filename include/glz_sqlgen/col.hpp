#pragma once

#include <string>
#include <string_view>
#include <glaze/util/string_literal.hpp>
#include "transpilation/Col.hpp"
#include "transpilation/As.hpp"
#include "transpilation/Desc.hpp"
#include "transpilation/Condition.hpp"
#include "transpilation/Operation.hpp"
#include "transpilation/Value.hpp"

namespace glz_sqlgen {

/// User-facing column template
/// Represents a column reference that can be used in queries
template <glz::string_literal Name, glz::string_literal Alias = "">
struct Col {
    using ColType = transpilation::Col<Name, Alias>;
    static constexpr std::string_view name = Name.sv();
    static constexpr std::string_view alias = Alias.sv();

    constexpr Col() = default;

    /// Implicit conversion to transpilation Col for to_sql() calls
    constexpr operator ColType() const noexcept {
        return ColType{};
    }

    /// Create an AS alias for this column
    template <glz::string_literal NewName>
    constexpr auto as() const noexcept {
        return transpilation::make_as<ColType, NewName>(ColType{});
    }

    /// Mark this column for descending sort order
    constexpr auto desc() const noexcept {
        return transpilation::make_desc(ColType{});
    }

    /// Returns the column name
    constexpr std::string_view get_name() const noexcept {
        return name;
    }

    // Comparison operators
    template <class T>
    friend constexpr auto operator==(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator!=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::not_equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator<(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::less_than>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator<=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::less_equal>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator>(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::greater_than>(ColType{}, Value<T>{rhs}));
    }

    template <class T>
    friend constexpr auto operator>=(const Col&, const T& rhs) {
        using namespace transpilation;
        return make_condition_wrapper(
            make_condition<Operator::greater_equal>(ColType{}, Value<T>{rhs}));
    }

    // Arithmetic operators
    template <class T>
    friend constexpr auto operator+(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::plus>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator-(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::minus>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator*(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::multiplies>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator/(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::divides>(ColType{}, Value<T>{rhs});
    }

    template <class T>
    friend constexpr auto operator%(const Col&, const T& rhs) noexcept {
        using namespace transpilation;
        return make_operation<Operator::mod>(ColType{}, Value<T>{rhs});
    }

    // Column-to-column operations
    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator==(const Col&, const Col<OtherName, OtherAlias>&) {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_condition_wrapper(
            make_condition<Operator::equal>(ColType{}, OtherColType{}));
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator!=(const Col&, const Col<OtherName, OtherAlias>&) {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_condition_wrapper(
            make_condition<Operator::not_equal>(ColType{}, OtherColType{}));
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator+(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::plus>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator-(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::minus>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator*(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::multiplies>(ColType{}, OtherColType{});
    }

    template <glz::string_literal OtherName, glz::string_literal OtherAlias>
    friend constexpr auto operator/(const Col&, const Col<OtherName, OtherAlias>&) noexcept {
        using namespace transpilation;
        using OtherColType = typename Col<OtherName, OtherAlias>::ColType;
        return make_operation<Operator::divides>(ColType{}, OtherColType{});
    }
};

/// Helper to create column references
template <glz::string_literal Name, glz::string_literal Alias = "">
inline constexpr Col<Name, Alias> col{};

} // namespace glz_sqlgen

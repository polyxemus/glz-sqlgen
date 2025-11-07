#pragma once

#include <glaze/util/string_literal.hpp>
#include <string_view>

namespace glz_sqlgen::transpilation {

/// Represents column aliasing (col AS alias_name)
template <class Value, glz::string_literal AliasName>
struct As {
    Value val;
    static constexpr std::string_view alias_name = AliasName.sv();

    constexpr As(const Value& v) : val(v) {}
    constexpr As(Value&& v) : val(std::move(v)) {}

    constexpr std::string_view get_alias() const noexcept {
        return alias_name;
    }

    const Value& get_value() const noexcept { return val; }
};

template <class Value, glz::string_literal AliasName>
constexpr auto make_as(Value&& val) {
    return As<std::remove_cvref_t<Value>, AliasName>{std::forward<Value>(val)};
}

} // namespace glz_sqlgen::transpilation

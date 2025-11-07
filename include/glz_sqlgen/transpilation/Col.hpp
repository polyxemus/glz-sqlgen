#pragma once

#include <string_view>
#include <glaze/util/string_literal.hpp>

namespace glz_sqlgen::transpilation {

/// Transpilation-level column representation
/// Used internally for SQL generation
template <glz::string_literal Name, glz::string_literal Alias = "">
struct Col {
    static constexpr std::string_view name = Name.sv();
    static constexpr std::string_view alias = Alias.sv();

    constexpr Col() = default;

    constexpr bool has_alias() const noexcept {
        return !alias.empty();
    }

    constexpr std::string_view get_name() const noexcept {
        return name;
    }

    constexpr std::string_view get_alias() const noexcept {
        return alias;
    }
};

} // namespace glz_sqlgen::transpilation

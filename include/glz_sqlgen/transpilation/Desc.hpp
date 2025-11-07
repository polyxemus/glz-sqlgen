#pragma once

namespace glz_sqlgen::transpilation {

/// Marker for descending sort order
template <class Col>
struct Desc {
    Col column;

    constexpr Desc(const Col& c) : column(c) {}
    constexpr Desc(Col&& c) : column(std::move(c)) {}

    const Col& get_column() const noexcept { return column; }
};

template <class Col>
constexpr auto make_desc(Col&& col) {
    return Desc<std::remove_cvref_t<Col>>{std::forward<Col>(col)};
}

} // namespace glz_sqlgen::transpilation

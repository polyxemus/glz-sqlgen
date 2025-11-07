#pragma once

namespace glz_sqlgen::transpilation {

/// Represents a SET clause for UPDATE (column = value)
template <class Col, class Value>
struct Set {
    Col column;
    Value value;

    constexpr Set(const Col& c, const Value& v) : column(c), value(v) {}
    constexpr Set(Col&& c, Value&& v) : column(std::move(c)), value(std::move(v)) {}
};

template <class Col, class Value>
constexpr auto make_set(Col&& col, Value&& val) {
    return Set<std::remove_cvref_t<Col>, std::remove_cvref_t<Value>>{
        std::forward<Col>(col), std::forward<Value>(val)};
}

} // namespace glz_sqlgen::transpilation

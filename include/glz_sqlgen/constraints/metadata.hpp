#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace glz_sqlgen::constraints {

/// Foreign key reference information
struct ForeignKeyReference {
    std::string table;
    std::string column;
    std::string on_delete = "";  // ON DELETE action (CASCADE, SET NULL, etc.)
    std::string on_update = "";  // ON UPDATE action
};

/// Constraint metadata for a field
struct FieldConstraints {
    bool is_primary_key = false;
    bool auto_increment = false;
    bool is_unique = false;
    bool is_not_null = false;
    std::optional<ForeignKeyReference> foreign_key;
    std::optional<size_t> max_length;      // For VARCHAR(N)
    std::optional<size_t> fixed_length;    // For CHAR(N)
    std::optional<std::string> pattern;    // Regex pattern for validation
    std::optional<std::string> default_value;

    // Specialized type flags
    bool is_json = false;
    bool is_timestamp = false;
    bool is_date = false;
    bool is_datetime = false;
    bool is_uuid = false;
    std::optional<std::string> timestamp_format;
};

} // namespace glz_sqlgen::constraints

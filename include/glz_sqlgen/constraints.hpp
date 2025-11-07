#pragma once

/// Convenience header including all constraint types
///
/// This header provides type-safe constraint wrappers for database columns:
/// - PrimaryKey<T, auto_incr>: Primary key with optional auto-increment
/// - Unique<T>: Unique constraint
/// - NotNull<T>: NOT NULL constraint
/// - ForeignKey<T, Table, "column">: Foreign key reference
/// - Varchar<N>: Variable-length string with max length
/// - Char<N>: Fixed-length string
///
/// Usage example:
/// ```cpp
/// struct User {
///     PrimaryKey<int, true> id;  // auto-incrementing primary key
///     Unique<std::string> username;
///     std::string email;
///     std::optional<std::string> bio;  // nullable field
/// };
/// ```

#include "constraints/metadata.hpp"
#include "constraints/traits.hpp"
#include "constraints/PrimaryKey.hpp"
#include "constraints/Unique.hpp"
#include "constraints/NotNull.hpp"
// ForeignKey, Varchar, Char will be added later

// Constraint types are already defined in glz_sqlgen namespace via their headers
// No additional exports needed

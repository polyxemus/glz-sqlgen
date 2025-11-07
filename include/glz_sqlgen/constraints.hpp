#pragma once

/// Convenience header including all constraint types
///
/// This header provides type-safe constraint wrappers for database columns:
///
/// **Basic Constraints:**
/// - PrimaryKey<T, auto_incr>: Primary key with optional auto-increment
/// - Unique<T>: Unique constraint
/// - NotNull<T>: NOT NULL constraint
/// - ForeignKey<T, Table, "column">: Foreign key reference
///
/// **Length-Constrained Types:**
/// - Varchar<N>: Variable-length string with max length
/// - Char<N>: Fixed-length string
///
/// **Specialized Types:**
/// - JSON<T>: JSON field with automatic serialization
/// - Date: Date-only type (YYYY-MM-DD)
/// - DateTime: Date and time type
/// - Timestamp: Unix timestamp
/// - UUID: Universal unique identifier
///
/// **Validated Types (SQL Injection Protection):**
/// - AlphaNumeric: Alphanumeric characters and underscores only
/// - Base64Encoded: Base64-encoded strings
/// - Email: Email address validation
/// - UUIDv4: UUID version 4 validation
/// - UrlSafe: URL-safe characters only
/// - Hex: Hexadecimal strings
///
/// Usage example:
/// ```cpp
/// struct User {
///     PrimaryKey<int, true> id;  // auto-incrementing primary key
///     Unique<Email> email;       // unique validated email
///     Varchar<50> username;      // max 50 characters
///     std::optional<std::string> bio;  // nullable field
///     Timestamp created_at;      // unix timestamp
/// };
/// ```

#include "constraints/metadata.hpp"
#include "constraints/traits.hpp"
#include "constraints/PrimaryKey.hpp"
#include "constraints/Unique.hpp"
#include "constraints/NotNull.hpp"
#include "constraints/ForeignKey.hpp"
#include "constraints/Varchar.hpp"
#include "constraints/Char.hpp"
#include "constraints/specialized_types.hpp"
#include "constraints/validated_types.hpp"

// Constraint types are already defined in glz_sqlgen namespace via their headers
// No additional exports needed

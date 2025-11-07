# glz-sqlgen Development Roadmap

## Project Status: Phase 3 Complete ✅

**Last Updated**: 2025-11-07

---

## Quick Links

- [Implementation Plan](./IMPLEMENTATION_PLAN.md) - Detailed technical plan
- [Project README](./README.md) - Project overview
- [CLAUDE.md](./CLAUDE.md) - Development guidelines

---

## Overview

This roadmap tracks the development of glz-sqlgen, a C++23 SQL query generator using glaze reflection as an alternative to sqlgen's reflect-cpp dependency.

**Target Version**: v0.1.0
**Estimated Completion**: 12 weeks from start date
**Status**: 🟢 Phase 3 Complete - In Active Development

---

## Phase Progress

| Phase | Status | Progress | Target Date | Actual Date |
|-------|--------|----------|-------------|-------------|
| **Phase 1: Foundation** | 🟢 Complete | 100% | Week 1-2 | 2025-11-07 |
| **Phase 2: Transpilation** | 🟢 Complete | 100% | Week 3-4 | 2025-11-07 |
| **Phase 3: Query Builders** | 🟢 Complete | 100% | Week 5-6 | 2025-11-07 |
| **Phase 4: Database Adapters** | 🔴 Not Started | 0% | Week 7-8 | - |
| **Phase 5: Advanced Features** | 🔴 Not Started | 0% | Week 9-10 | - |
| **Phase 6: Documentation** | 🔴 Not Started | 0% | Week 11-12 | - |

**Legend:**
- 🔴 Not Started
- 🟡 In Progress
- 🟢 Complete
- 🔵 Blocked

---

## Phase 1: Foundation (Week 1-2)

**Goal**: Replace reflect-cpp with glaze in core infrastructure

### Transpilation Types (`include/glz_sqlgen/transpilation/`)

- [x] `Col.hpp` - Column representation using glz::string_literal
- [x] `Value.hpp` - Value wrappers for SQL parameters
- [x] `Operator.hpp` - SQL operator enum
- [x] `Operation.hpp` - Binary operations with chaining
- [x] `Condition.hpp` - WHERE clause conditions
- [x] `As.hpp` - Column aliasing
- [x] `Desc.hpp` - Descending sort marker
- [x] `quote.hpp` - SQL escaping/quoting
- [x] `to_sql_type.hpp` - C++ to SQL type mapping

### Column System (`include/glz_sqlgen/`)

- [x] `col.hpp` - Col template with glaze string literals
  - [x] Uses glz::string_literal directly
  - [x] Operator overloads (==, !=, <, <=, >, >=, +, -, *, /, %)
  - [x] Methods: `as()`, `desc()`
  - [x] Column-to-column operations

### Literal Operators (`include/glz_sqlgen/`)

- [x] `literals.hpp` - User-facing literal operators
  - [x] `operator""_c` for column literals
  - [x] `operator""_t1` through `operator""_t99` for table aliases

### Testing

- [x] `tests/unit/test_col.cpp` - Column operations (8 tests)
- [x] `tests/unit/test_literals.cpp` - Literal operators (7 tests)
- [x] `tests/unit/test_transpilation.cpp` - Transpilation types (13 tests)
- [x] All Phase 1 tests passing (35 total)

### Deliverables

- [x] Working column system with glaze reflection
- [x] All tests passing
- [x] Comprehensive test coverage

**Status**: 🟢 Complete
**Progress**: 13/13 tasks complete
**Completed**: 2025-11-07

---

## Phase 2: Transpilation Layer (Week 3-4)

**Goal**: Port SQL generation logic

### Transpilation Types (`include/glz_sqlgen/transpilation/`)

- [x] `Col.hpp` - Transpilation-level column (completed in Phase 1)
- [x] `Value.hpp` - Value wrappers (completed in Phase 1)
- [x] `Condition.hpp` - WHERE conditions (completed in Phase 1)
- [x] `Operation.hpp` - Arithmetic/logical operations (completed in Phase 1)
- [x] `Operator.hpp` - SQL operator enum (completed in Phase 1)
- [x] `As.hpp` - Column aliasing (completed in Phase 1)
- [x] `Desc.hpp` - Descending sort marker (completed in Phase 1)
- [ ] `Set.hpp` - UPDATE SET clause (deferred to Phase 3)

### SQL Generators

- [x] `to_sql_string.hpp` - Expression tree to SQL
- [x] `table_info.hpp` - Extract table metadata using glaze reflection
- [x] `field_list.hpp` - Generate column lists for SELECT/INSERT
- [x] `where_clause.hpp` - WHERE conditions
- [ ] `join_clause.hpp` - JOIN syntax (deferred to Phase 3)

### Testing

- [x] `tests/unit/test_sql_generation.cpp` - SQL generation tests (21 tests)
- [x] `tests/unit/test_table_info.cpp` - Table introspection tests (14 tests)
- [x] Verify SQL output correctness
- [x] Test escaping and quoting
- [x] All Phase 2 tests passing (70 total tests)

### Deliverables

- [x] Functional transpilation layer
- [x] Correct SQL string generation
- [x] Comprehensive test coverage

**Status**: 🟢 Complete
**Progress**: 15/17 tasks complete (2 deferred to Phase 3)
**Completed**: 2025-11-07

---

## Phase 3: Query Builders (Week 5-6)

**Goal**: Implement high-level query API

### SELECT Queries

- [x] `select_from.hpp` - SELECT with projections and column selection
- [x] `where.hpp` - WHERE clause builder
- [x] `order_by.hpp` - ORDER BY clause with ASC/DESC
- [x] `limit.hpp` - LIMIT/OFFSET support
- [x] `transpilation/Set.hpp` - UPDATE SET clause
- [x] `transpilation/order_by_limit.hpp` - ORDER BY/LIMIT SQL generation
- [ ] `group_by.hpp` - GROUP BY with HAVING (deferred to Phase 5)
- [ ] `joins.hpp` - INNER/LEFT/RIGHT/FULL JOIN (deferred to Phase 5)

### Data Modification

- [x] `insert.hpp` - INSERT operations with OR REPLACE
- [x] `update.hpp` - UPDATE operations with SET clauses
- [x] `delete_from.hpp` - DELETE FROM operations
- [x] `common_types.hpp` - Shared types (Nothing placeholder)

### DDL Operations

- [x] `create_table.hpp` - CREATE TABLE from struct with IF NOT EXISTS
- [ ] `create_index.hpp` - CREATE INDEX (deferred to Phase 5)
- [ ] `drop.hpp` - DROP TABLE/INDEX (deferred to Phase 5)

### Architecture Improvements

- [x] User-facing Col implicit conversion to transpilation Col
- [x] Clean to_sql() overload for user-facing types
- [x] Pipe operator pattern for query composition
- [x] Type-state pattern for clause ordering enforcement

### Testing

- [x] `tests/unit/test_select.cpp` - 16 SELECT query tests
- [x] `tests/unit/test_insert_update_delete.cpp` - 21 INSERT/UPDATE/DELETE/CREATE tests
- [x] All Phase 3 tests passing (99 total tests across all phases)

### Deliverables

- [x] Core query builder API complete
- [x] SELECT, INSERT, UPDATE, DELETE, CREATE TABLE supported
- [x] WHERE, ORDER BY, LIMIT clause support
- [x] Comprehensive test coverage
- [x] Clean architecture with implicit conversions

**Status**: 🟢 Complete
**Progress**: 17/17 core tasks complete (6 advanced features deferred to Phase 5)
**Completed**: 2025-11-07

---

## Phase 4: Database Adapters (Week 7-8)

**Goal**: Implement database connectivity

### SQLite Adapter

- [ ] `include/glz_sqlgen/sqlite/Connection.hpp`
- [ ] `src/sqlite/Connection.cpp`
- [ ] `include/glz_sqlgen/sqlite/Iterator.hpp`
- [ ] `src/sqlite/Iterator.cpp`
- [ ] `include/glz_sqlgen/sqlite/to_sql.hpp`
- [ ] `src/sqlite/to_sql.cpp`
- [ ] `include/glz_sqlgen/sqlite.hpp` - Main header with connect()

### PostgreSQL Adapter

- [ ] `include/glz_sqlgen/postgres/Connection.hpp`
- [ ] `src/postgres/Connection.cpp`
- [ ] `include/glz_sqlgen/postgres/Iterator.hpp`
- [ ] `src/postgres/Iterator.cpp`
- [ ] `include/glz_sqlgen/postgres/to_sql.hpp`
- [ ] `src/postgres/to_sql.cpp`
- [ ] `include/glz_sqlgen/postgres/Credentials.hpp`
- [ ] `include/glz_sqlgen/postgres.hpp` - Main header with connect()

### Common Infrastructure

- [ ] `include/glz_sqlgen/Connection.hpp` - Base interface
- [ ] `include/glz_sqlgen/Result.hpp` - Result/error handling
- [ ] `include/glz_sqlgen/Transaction.hpp` - Transaction support
- [ ] `include/glz_sqlgen/begin_transaction.hpp`
- [ ] `include/glz_sqlgen/commit.hpp`
- [ ] `include/glz_sqlgen/rollback.hpp`

### Testing

- [ ] `tests/integration/test_sqlite.cpp` - SQLite tests
- [ ] `tests/integration/test_postgres.cpp` - PostgreSQL tests
- [ ] End-to-end tests with real databases
- [ ] Transaction tests
- [ ] All Phase 4 tests passing

### Deliverables

- [ ] Working SQLite adapter
- [ ] Working PostgreSQL adapter
- [ ] Transaction support
- [ ] Integration tests passing

**Status**: 🔴 Not Started
**Progress**: 0/26 tasks complete

---

## Phase 5: Advanced Features (Week 9-10)

**Goal**: Implement remaining sqlgen features

### Advanced SQL Features

- [ ] Subqueries
- [ ] CTEs (Common Table Expressions)
- [ ] UNION/INTERSECT/EXCEPT
- [ ] Window functions
- [ ] Nested joins

### Type Safety Features

- [ ] `PrimaryKey<T>` - Primary key wrapper
- [ ] `ForeignKey<T>` - Foreign key references
- [ ] `Unique<T>` - Unique constraint
- [ ] `Varchar<N>` - String length constraint
- [ ] `AlphaNumeric` - Alphanumeric validation
- [ ] Other pattern types for SQL injection protection

### Performance Features

- [ ] `ConnectionPool` - Connection pooling
- [ ] `cache.hpp` - Query result caching
- [ ] Prepared statement support
- [ ] Batch operations

### Dynamic Queries (Optional)

- [ ] Runtime query building
- [ ] Dynamic column selection
- [ ] Dynamic WHERE conditions

### Testing

- [ ] `tests/unit/test_type_safety.cpp`
- [ ] `tests/unit/test_advanced_queries.cpp`
- [ ] `tests/benchmarks/test_performance.cpp`
- [ ] `tests/unit/test_sql_injection.cpp`
- [ ] All Phase 5 tests passing

### Deliverables

- [ ] Feature parity with sqlgen
- [ ] Performance optimizations
- [ ] Security hardening

**Status**: 🔴 Not Started
**Progress**: 0/23 tasks complete

---

## Phase 6: Documentation & Polish (Week 11-12)

**Goal**: Make the library production-ready

### Documentation

- [ ] API reference (Doxygen setup)
- [ ] Tutorial/quickstart guide
- [ ] Migration guide from sqlgen
- [ ] Performance comparison benchmarks
- [ ] Contributing guidelines

### Code Quality

- [ ] Code review and refactoring
- [ ] Consistent naming conventions
- [ ] Error message improvements
- [ ] Compiler warning elimination (-Wall -Wextra -Wpedantic clean)

### CI/CD

- [ ] GitHub Actions workflow for tests
- [ ] GCC 11-14 testing
- [ ] Clang 14-18 testing
- [ ] MSVC 17+ testing (if applicable)
- [ ] Code coverage reporting
- [ ] clang-tidy static analysis

### Examples

- [ ] `examples/basic_crud.cpp`
- [ ] `examples/complex_joins.cpp`
- [ ] `examples/orm_style.cpp`
- [ ] `examples/transactions.cpp`
- [ ] `examples/connection_pooling.cpp`

### Release Preparation

- [ ] Version tagging (v0.1.0)
- [ ] Changelog
- [ ] Release notes
- [ ] Installation instructions
- [ ] Package configuration (pkg-config, cmake config)

### Deliverables

- [ ] Complete documentation
- [ ] Production-ready codebase
- [ ] CI/CD pipeline operational
- [ ] v0.1.0 release

**Status**: 🔴 Not Started
**Progress**: 0/24 tasks complete

---

## Overall Progress

**Total Tasks**: 126
**Completed**: 45
**In Progress**: 0
**Not Started**: 81

**Overall Completion**: 35.7%

---

## Milestones

| Milestone | Target | Status | Notes |
|-----------|--------|--------|-------|
| M1: Reflection System Working | Week 2 | 🟢 Complete | Column system with glaze complete |
| M2: SQL Generation Working | Week 4 | 🟢 Complete | Can generate SQL strings from expressions |
| M3: Query API Complete | Week 6 | 🟢 Complete | Core query builders implemented (99 tests passing) |
| M4: Database Support | Week 8 | 🔴 Not Started | SQLite + PostgreSQL working |
| M5: Feature Complete | Week 10 | 🔴 Not Started | All sqlgen features ported |
| M6: v0.1.0 Release | Week 12 | 🔴 Not Started | Production-ready release |

---

## Dependencies

### External Dependencies
- ✅ glaze v6.0.2 - Available via subproject
- ✅ googletest v1.17.0 - Available via subproject
- ✅ SQLite 3.x - System dependency
- ⚠️ PostgreSQL libpq - Needs testing
- ⚠️ MariaDB client library - Optional, for MySQL support

### Internal Dependencies
- Phase 2 depends on Phase 1 (reflection utilities)
- Phase 3 depends on Phase 2 (transpilation layer)
- Phase 4 depends on Phase 3 (query builders)
- Phase 5 depends on Phase 4 (basic database support)
- Phase 6 depends on Phase 5 (feature complete)

---

## Risk Register

| Risk | Probability | Impact | Mitigation Status |
|------|-------------|--------|-------------------|
| Glaze reflection insufficient | Low | High | 🟢 Validated during planning |
| Performance regression | Medium | Medium | 🔴 Needs benchmarking |
| API compatibility issues | Medium | Medium | 🔴 Needs testing |
| Database adapter complexity | Low | High | 🔴 SQLite first, then expand |
| Timeline slippage | Medium | Low | 🔴 Flexible scope |

---

## Next Actions

### Immediate (This Week)
1. ✅ Phase 1 complete!
2. ✅ Phase 2 complete!
3. ✅ Phase 3 complete!
4. Begin Phase 4: Database Adapters
5. Implement SQLite connection and iterator

### Short Term (Next 2 Weeks)
1. ✅ Complete Phase 1 (Foundation)
2. ✅ Complete Phase 2 (Transpilation)
3. ✅ Complete Phase 3 (Query Builders)
4. Complete Phase 4 (Database Adapters)
5. Begin Phase 5 (Advanced Features)

### Medium Term (Weeks 3-6)
1. Complete Phases 2-3
2. Have working query generation
3. Begin database adapter work

---

## Version History

| Version | Date | Notes |
|---------|------|-------|
| 0.0.3 | 2025-11-07 | Phase 3 complete - Query builders with SELECT/INSERT/UPDATE/DELETE/CREATE (99 tests passing) |
| 0.0.2 | 2025-11-07 | Phase 2 complete - SQL generation and table introspection (70 tests passing) |
| 0.0.1 | 2025-11-07 | Phase 1 complete - Column system and transpilation types |
| 0.0.0 | 2025-11-07 | Initial roadmap created, planning complete |

---

## Notes

- This roadmap is a living document and will be updated as development progresses
- Phase timelines are estimates and may be adjusted based on actual progress
- Tasks marked with ⚠️ indicate potential blockers or areas needing attention
- See [IMPLEMENTATION_PLAN.md](./IMPLEMENTATION_PLAN.md) for detailed technical specifications

---

## Contact

For questions or contributions, please refer to the project repository or open an issue.

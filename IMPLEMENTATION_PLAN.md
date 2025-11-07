# Implementation Plan: glz-sqlgen

## SQL Query Generator with Glaze Reflection

### Executive Summary

This plan outlines the implementation of `glz-sqlgen`, a reimplementation of [sqlgen](https://github.com/getml/sqlgen) using [glaze](https://github.com/stephenberry/glaze) for reflection instead of reflect-cpp. The project will maintain API compatibility with sqlgen while leveraging glaze's simpler, faster compile-time reflection.

---

## 1. Architecture Analysis

### 1.1 sqlgen Architecture (Current)

**Core Components:**
- **Column System**: Type-safe column references via `Col<name, alias>` template
- **Literal Operators**: User-facing literals (`"name"_c`, `"name"_t1`, etc.)
- **Transpilation Layer**: Converts query expressions to SQL strings
  - `transpilation::Col`, `transpilation::Condition`, `transpilation::Operation`
  - Expression tree building via operator overloading
- **Query Builders**:
  - `select_from<T>` - SELECT queries with joins, aggregations
  - `read<T>` - Simple SELECT all queries
  - `write()` - INSERT operations with auto-table creation
  - `update()`, `delete_from()`, etc.
- **Database Adapters**: PostgreSQL, MySQL, SQLite implementations
- **Reflection Usage**: Uses `rfl::internal::StringLiteral` and reflect-cpp's field iteration

**Key Dependencies on reflect-cpp:**
```cpp
// Field iteration
rfl::fields<T>
rfl::to_named_tuple<T>
rfl::from_named_tuple<T>

// String literals
rfl::internal::StringLiteral

// Type inspection
rfl::Literal<"name">
```

### 1.2 glaze Reflection Capabilities

**Core Features:**
- **Pure Reflection**: Automatic structured binding decomposition
  ```cpp
  struct Person { std::string name; int age; };
  // Automatically reflected - no macros needed!
  auto tie = glz::to_tie(person); // Returns tuple of references
  ```

- **Field Name Extraction**: Compile-time name retrieval
  ```cpp
  glz::member_nameof<0, Person> // "name"
  glz::member_nameof<1, Person> // "age"
  ```

- **Custom Metadata**: `glz::meta<T>` specialization
  ```cpp
  template <>
  struct glz::meta<Person> {
    using T = Person;
    static constexpr auto value = glz::object(
      "name", &T::name,
      "age", &T::age
    );
  };
  ```

- **Field Count**: `glz::detail::count_members<T>`
- **Type Names**: `glz::name_v<T>` for type name strings

**Key Advantages:**
- ✅ Header-only, zero runtime overhead
- ✅ Simpler than reflect-cpp (fewer template layers)
- ✅ Supports up to 128 fields via structured bindings
- ✅ 3-4x faster compilation than reflect-cpp (per glaze benchmarks)
- ✅ C++20/23 native (already required by project)

---

## 2. Migration Strategy

### 2.1 String Literal Replacement

**Current (sqlgen with reflect-cpp):**
```cpp
template <rfl::internal::StringLiteral _name>
struct Col { ... };
```

**New (glz-sqlgen with glaze):**
```cpp
// Option 1: Use glz::string_literal
template <glz::string_literal _name>
struct Col { ... };

// Option 2: Define our own compatible type
namespace glz_sqlgen {
  template <size_t N>
  struct string_literal {
    constexpr string_literal(const char (&str)[N]) {
      std::copy_n(str, N, value);
    }
    char value[N];
    static constexpr size_t size = N - 1;
    constexpr operator std::string_view() const {
      return {value, size};
    }
  };
}
```

**Impact**: Low - Mostly mechanical replacement

### 2.2 Field Iteration

**Current (sqlgen):**
```cpp
rfl::to_named_tuple<Person>(person)
// Returns: rfl::NamedTuple<"name", std::string, "age", int>
```

**New (glz-sqlgen):**
```cpp
// Approach 1: Use glz::to_tie + index iteration
template <class T>
constexpr void for_each_field(T&& obj, auto&& func) {
  auto tie = glz::to_tie(obj);
  [&]<size_t... Is>(std::index_sequence<Is...>) {
    (func.template operator()<Is>(glz::get<Is>(tie),
                                   glz::member_nameof<Is, T>), ...);
  }(std::make_index_sequence<glz::tuple_size_v<decltype(tie)>>{});
}

// Approach 2: Use glz::meta if custom metadata needed
template <class T>
void iterate_columns() {
  if constexpr (glz::glaze_object_t<T>) {
    constexpr auto& meta = glz::meta<T>::value;
    // Iterate over meta tuple
  }
}
```

**Impact**: Medium - Requires helper utilities

### 2.3 Type Name Extraction

**Current:**
```cpp
rfl::type_name<T>() // Returns "Person"
```

**New:**
```cpp
glz::name_v<T> // Returns compile-time string of type name
// Or use custom table name via glz::meta
template <>
struct glz::meta<Person> {
  static constexpr std::string_view name = "Person";
};
```

**Impact**: Low - Direct replacement available

---

## 3. Implementation Phases

### Phase 1: Foundation (Week 1-2)

**Goal**: Replace reflect-cpp with glaze in core infrastructure

**Tasks:**
1. **Create Reflection Utilities** (`include/glz_sqlgen/reflection/`)
   - [ ] `field_traits.hpp` - Field count, names, types extraction
   - [ ] `field_iterator.hpp` - Iterate over struct fields
   - [ ] `string_literal.hpp` - Compatible string literal type
   - [ ] `type_name.hpp` - Extract type names for table names

2. **Port Column System** (`include/glz_sqlgen/col.hpp`)
   - [ ] Replace `rfl::internal::StringLiteral` with glaze equivalent
   - [ ] Keep `Col<name, alias>` template structure
   - [ ] Maintain operator overloads (==, !=, <, >, +, -, *, /, etc.)
   - [ ] Port `as()`, `desc()`, `is_null()`, `like()`, etc. methods

3. **Port Literal Operators** (`include/glz_sqlgen/literals.hpp`)
   - [ ] Implement `operator""_c` for column literals
   - [ ] Generate `_t1`, `_t2`, ... `_t99` table alias literals
   - [ ] Test compile-time correctness

4. **Testing**
   - [ ] Create `test_reflection.cpp` - Verify field iteration works
   - [ ] Create `test_col.cpp` - Test column operations
   - [ ] Create `test_literals.cpp` - Test literal operators

**Deliverables:**
- Working column system with glaze reflection
- All tests passing for core reflection utilities

---

### Phase 2: Transpilation Layer (Week 3-4)

**Goal**: Port SQL generation logic

**Tasks:**
1. **Port Transpilation Types** (`include/glz_sqlgen/transpilation/`)
   - [ ] `Col.hpp` - Transpilation-level column representation
   - [ ] `Value.hpp` - Value wrappers for parameters
   - [ ] `Condition.hpp` - WHERE clause conditions
   - [ ] `Operation.hpp` - Arithmetic/logical operations
   - [ ] `Operator.hpp` - SQL operator enum
   - [ ] `As.hpp` - Column aliasing
   - [ ] `Desc.hpp` - Descending sort marker

2. **Create SQL Generators** (`include/glz_sqlgen/transpilation/`)
   - [ ] `to_sql_string.hpp` - Convert expression trees to SQL
   - [ ] `table_info.hpp` - Extract table metadata from types
   - [ ] `field_list.hpp` - Generate column lists
   - [ ] `where_clause.hpp` - Generate WHERE conditions
   - [ ] `join_clause.hpp` - Generate JOIN syntax

3. **Testing**
   - [ ] `test_transpilation.cpp` - Unit tests for SQL generation
   - [ ] Verify correct SQL output for various expressions
   - [ ] Test escaping and quoting

**Deliverables:**
- Fully functional transpilation layer
- SQL string generation working correctly

---

### Phase 3: Query Builders (Week 5-6)

**Goal**: Implement high-level query API

**Tasks:**
1. **SELECT Queries** (`include/glz_sqlgen/`)
   - [ ] `select_from.hpp` - SELECT with projections, joins, aggregations
   - [ ] `read.hpp` - Simple `read<std::vector<T>>()` helper
   - [ ] `where.hpp` - WHERE clause builder
   - [ ] `order_by.hpp` - ORDER BY clause
   - [ ] `group_by.hpp` - GROUP BY with HAVING
   - [ ] `limit.hpp` - LIMIT/OFFSET support
   - [ ] `joins.hpp` - INNER/LEFT/RIGHT/FULL JOIN

2. **Data Modification** (`include/glz_sqlgen/`)
   - [ ] `insert.hpp` - INSERT operations
   - [ ] `update.hpp` - UPDATE operations
   - [ ] `delete_from.hpp` - DELETE operations
   - [ ] `write.hpp` - High-level write helper (auto-creates table)

3. **DDL Operations** (`include/glz_sqlgen/`)
   - [ ] `create_table.hpp` - CREATE TABLE from struct
   - [ ] `create_index.hpp` - CREATE INDEX
   - [ ] `drop.hpp` - DROP TABLE/INDEX
   - [ ] `if_exists.hpp`, `if_not_exists.hpp` - Conditional DDL

4. **Aggregations** (`include/glz_sqlgen/`)
   - [ ] `aggregations.hpp` - COUNT, SUM, AVG, MIN, MAX

5. **Testing**
   - [ ] `test_select.cpp` - SELECT query tests
   - [ ] `test_insert.cpp` - INSERT tests
   - [ ] `test_update_delete.cpp` - UPDATE/DELETE tests
   - [ ] `test_joins.cpp` - JOIN query tests
   - [ ] `test_aggregations.cpp` - Aggregation function tests

**Deliverables:**
- Complete query builder API
- All query types supported (SELECT, INSERT, UPDATE, DELETE)
- Comprehensive test coverage

---

### Phase 4: Database Adapters (Week 7-8)

**Goal**: Implement database connectivity

**Tasks:**
1. **SQLite Adapter** (`src/sqlite/`, `include/glz_sqlgen/sqlite/`)
   - [ ] `Connection.hpp/.cpp` - Connection management
   - [ ] `Iterator.hpp/.cpp` - Result iteration
   - [ ] `to_sql.hpp/.cpp` - SQLite-specific SQL generation
   - [ ] `connect()` - Connection factory

2. **PostgreSQL Adapter** (`src/postgres/`, `include/glz_sqlgen/postgres/`)
   - [ ] `Connection.hpp/.cpp` - libpq integration
   - [ ] `Iterator.hpp/.cpp` - Result handling
   - [ ] `to_sql.hpp/.cpp` - PostgreSQL-specific SQL
   - [ ] `Credentials.hpp` - Connection credentials
   - [ ] `connect()` - Connection factory

3. **MySQL Adapter** (Optional - `src/mysql/`)
   - [ ] Similar structure to PostgreSQL
   - [ ] Uses libmariadb

4. **Common Infrastructure**
   - [ ] `Connection.hpp` - Base connection interface
   - [ ] `Result.hpp` - Result/error handling
   - [ ] `Transaction.hpp` - Transaction support
   - [ ] `begin_transaction()`, `commit()`, `rollback()`

5. **Testing**
   - [ ] `test_sqlite.cpp` - SQLite integration tests
   - [ ] `test_postgres.cpp` - PostgreSQL integration tests
   - [ ] End-to-end tests with real databases
   - [ ] Connection pooling tests

**Deliverables:**
- Working SQLite adapter (minimum)
- PostgreSQL adapter
- Transaction support
- Integration tests passing

---

### Phase 5: Advanced Features (Week 9-10)

**Goal**: Implement remaining sqlgen features

**Tasks:**
1. **Advanced SQL Features**
   - [ ] Subqueries and CTEs (Common Table Expressions)
   - [ ] UNION/INTERSECT/EXCEPT
   - [ ] Window functions
   - [ ] Nested joins

2. **Type Safety Features**
   - [ ] `PrimaryKey<T>` - Primary key wrapper
   - [ ] `ForeignKey<T>` - Foreign key references
   - [ ] `Unique<T>` - Unique constraint
   - [ ] `Varchar<N>` - String length constraint
   - [ ] Pattern types (`AlphaNumeric`, `Email`, etc.) for SQL injection protection

3. **Performance Features**
   - [ ] `ConnectionPool` - Connection pooling
   - [ ] `cache.hpp` - Query result caching
   - [ ] Prepared statement support
   - [ ] Batch operations

4. **Dynamic Queries** (`include/glz_sqlgen/dynamic/`)
   - [ ] Runtime query building (optional)
   - [ ] Dynamic column selection
   - [ ] Dynamic WHERE conditions

5. **Testing**
   - [ ] `test_type_safety.cpp` - Type safety features
   - [ ] `test_advanced_queries.cpp` - Complex queries
   - [ ] `test_performance.cpp` - Performance benchmarks
   - [ ] `test_sql_injection.cpp` - Security tests

**Deliverables:**
- Feature parity with sqlgen
- Performance optimizations
- Security hardening

---

### Phase 6: Documentation & Polish (Week 11-12)

**Goal**: Make the library production-ready

**Tasks:**
1. **Documentation**
   - [ ] API reference documentation (Doxygen)
   - [ ] Tutorial/quickstart guide
   - [ ] Migration guide from sqlgen
   - [ ] Examples directory with common patterns
   - [ ] Performance comparison benchmarks

2. **Code Quality**
   - [ ] Code review and refactoring
   - [ ] Consistent naming conventions
   - [ ] Error message improvements
   - [ ] Compiler warning elimination

3. **CI/CD**
   - [ ] GitHub Actions for automated testing
   - [ ] Multi-compiler testing (GCC, Clang, MSVC)
   - [ ] Code coverage reporting
   - [ ] Static analysis (clang-tidy)

4. **Examples**
   - [ ] Basic CRUD operations example
   - [ ] Complex joins example
   - [ ] ORM-style usage example
   - [ ] Transaction handling example
   - [ ] Connection pooling example

**Deliverables:**
- Complete documentation
- Production-ready codebase
- CI/CD pipeline
- Example applications

---

## 4. Technical Decisions

### 4.1 Namespace Strategy
- Use `glz_sqlgen` namespace throughout
- Maintain internal namespace structure similar to sqlgen:
  - `glz_sqlgen::transpilation` - SQL generation internals
  - `glz_sqlgen::literals` - User-facing literals
  - `glz_sqlgen::sqlite`, `postgres`, `mysql` - Database adapters
  - `glz_sqlgen::internal` - Implementation details

### 4.2 API Compatibility
**Maintain compatibility where possible:**
```cpp
// These should work identically to sqlgen
using namespace glz_sqlgen::literals;
auto query = read<std::vector<Person>> |
             where("age"_c >= 18) |
             order_by("name"_c) |
             limit(10);
const auto result = query(conn);
```

**Breaking changes (minimal):**
- Include paths: `#include <glz_sqlgen/...>` instead of `#include <sqlgen/...>`
- Namespace: `glz_sqlgen::` instead of `sqlgen::`
- Some internal APIs may differ due to glaze vs reflect-cpp

### 4.3 Glaze Meta Usage

For custom table/column names:
```cpp
struct Person {
  std::string first_name;
  int age;
};

// Optional: Customize names
template <>
struct glz::meta<Person> {
  static constexpr std::string_view name = "people"; // table name
  using T = Person;
  static constexpr auto value = glz::object(
    "firstName", &T::first_name,  // SQL column name override
    "age", &T::age
  );
};
```

### 4.4 Error Handling
- Maintain `Result<T>` pattern from sqlgen (uses `std::expected`-like type)
- No exceptions in core library
- Clear error messages with context

### 4.5 Performance Targets
- Compile-time overhead: **< sqlgen** (due to simpler glaze reflection)
- Runtime overhead: **~= sqlgen** (same SQL generation approach)
- Binary size: **< sqlgen** (fewer template instantiations)

---

## 5. Testing Strategy

### 5.1 Test Structure
```
tests/
├── unit/                    # Unit tests for individual components
│   ├── test_reflection.cpp  # Reflection utilities
│   ├── test_col.cpp        # Column system
│   ├── test_literals.cpp    # Literal operators
│   ├── test_transpilation.cpp
│   └── ...
├── integration/             # Integration tests with databases
│   ├── test_sqlite_integration.cpp
│   ├── test_postgres_integration.cpp
│   └── ...
├── examples/                # Example programs
│   ├── basic_crud.cpp
│   ├── advanced_queries.cpp
│   └── ...
└── benchmarks/              # Performance benchmarks
    ├── compile_time.cpp
    ├── runtime.cpp
    └── comparison.cpp       # vs sqlgen
```

### 5.2 Test Coverage Goals
- Unit test coverage: **> 90%**
- Integration test coverage: **> 80%**
- All examples must compile and run successfully

### 5.3 Continuous Integration
- Run tests on: GCC 11-14, Clang 14-18, MSVC 17+
- Test on: Linux, Windows, macOS
- Database versions: SQLite 3.45+, PostgreSQL 14-16

---

## 6. Risk Mitigation

| Risk | Impact | Mitigation |
|------|---------|-----------|
| Glaze API changes | High | Pin to specific glaze version (v6.0.2), track upstream |
| Incomplete reflection capabilities | High | Prototype early, validate all needed features |
| Performance regression | Medium | Benchmark early and often vs sqlgen |
| API incompatibility | Medium | Maintain compatibility layer for common patterns |
| Complex query bugs | Medium | Extensive testing, SQL validation |

---

## 7. Success Criteria

1. ✅ **Feature Parity**: All core sqlgen features implemented
2. ✅ **Performance**: Compile times ≤ sqlgen, runtime performance ~= sqlgen
3. ✅ **API Compatibility**: 90%+ of sqlgen examples work with minimal changes
4. ✅ **Test Coverage**: >90% unit test coverage, all integration tests pass
5. ✅ **Documentation**: Complete API docs and tutorial
6. ✅ **Production Ready**: Used successfully in at least one real project

---

## 8. Timeline Summary

| Phase | Duration | Key Deliverable |
|-------|----------|-----------------|
| Phase 1: Foundation | 2 weeks | Reflection utilities, column system |
| Phase 2: Transpilation | 2 weeks | SQL generation |
| Phase 3: Query Builders | 2 weeks | Complete query API |
| Phase 4: Database Adapters | 2 weeks | SQLite, PostgreSQL support |
| Phase 5: Advanced Features | 2 weeks | Feature parity |
| Phase 6: Documentation | 2 weeks | Production-ready release |
| **Total** | **12 weeks** | **v0.1.0 Release** |

---

## 9. References

- **sqlgen**: https://github.com/getml/sqlgen
- **glaze**: https://github.com/stephenberry/glaze
- **reflect-cpp**: https://github.com/getml/reflect-cpp
- **C++23 Standard**: https://en.cppreference.com/w/cpp/23

# Test sources
test_sources = files(
  'test_query_builder.cpp',
  'unit/test_col.cpp',
  'unit/test_literals.cpp',
  'unit/test_transpilation.cpp',
  'unit/test_sql_generation.cpp',
  'unit/test_table_info.cpp',
  'unit/test_select.cpp',
  'unit/test_insert_update_delete.cpp',
  'unit/test_joins.cpp',
  'unit/test_aggregates.cpp',
  'unit/test_advanced_conditions.cpp',
  'unit/test_string_functions.cpp',
  'unit/test_math_functions.cpp',
  'unit/test_date_functions.cpp',
  'integration/test_sqlite.cpp',
)

# Test executable
test_exe = executable(
  'glz_sqlgen_tests',
  sources: test_sources,
  include_directories: inc_dir,
  dependencies: [gtest_dep, gtest_main_dep, glz_sqlgen_dep],
  cpp_args: ['-Wno-sign-compare'],  # Suppress warnings from googletest headers
)

# Register test with meson
test('glz_sqlgen_tests', test_exe)

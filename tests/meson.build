# Test sources
test_sources = files(
  'test_query_builder.cpp',
)

# Test executable
test_exe = executable(
  'glz_sqlgen_tests',
  sources: test_sources,
  include_directories: inc_dir,
  dependencies: [gtest_dep, gtest_main_dep, glz_sqlgen_dep],
)

# Register test with meson
test('glz_sqlgen_tests', test_exe)

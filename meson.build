project('glz-sqlgen', 'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=true',
  ],
)

# Dependencies
glaze_dep = dependency('glaze', fallback: ['glaze', 'glaze_glaze_dep'])
sqlgen_dep = dependency('sqlgen', fallback: ['sqlgen', 'sqlgen_dep'])
gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'], required: false)
gtest_main_dep = dependency('gtest_main', fallback: ['gtest', 'gtest_main_dep'], required: false)

# Include directories
inc_dir = include_directories('include')

# Library sources
sources = files(
  'src/query_builder.cpp',
)

# Library
glz_sqlgen_lib = library('glz-sqlgen',
  sources: sources,
  include_directories: inc_dir,
  dependencies: [glaze_dep, sqlgen_dep],
  install: true,
)

# Make this library usable as a dependency
glz_sqlgen_dep = declare_dependency(
  include_directories: inc_dir,
  link_with: glz_sqlgen_lib,
  dependencies: [glaze_dep, sqlgen_dep],
)

# Tests
if gtest_dep.found() and gtest_main_dep.found()
  subdir('tests')
endif

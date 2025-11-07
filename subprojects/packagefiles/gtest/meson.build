project('googletest', 'cpp',
  version: '1.17.0',
  default_options: [
    'cpp_std=c++17',
  ],
)

thread_dep = dependency('threads')

gtest_inc = include_directories('googletest/include')
gtest_src_inc = include_directories('googletest')
gmock_inc = include_directories('googlemock/include')
gmock_src_inc = include_directories('googlemock')

# GTest library
gtest_lib = static_library(
  'gtest',
  'googletest/src/gtest-all.cc',
  include_directories: [gtest_inc, gtest_src_inc],
  dependencies: thread_dep,
)

gtest_dep = declare_dependency(
  include_directories: gtest_inc,
  link_with: gtest_lib,
  dependencies: thread_dep,
)

# GTest main library
gtest_main_lib = static_library(
  'gtest_main',
  'googletest/src/gtest_main.cc',
  include_directories: [gtest_inc, gtest_src_inc],
  link_with: gtest_lib,
  dependencies: thread_dep,
)

gtest_main_dep = declare_dependency(
  include_directories: gtest_inc,
  link_with: [gtest_lib, gtest_main_lib],
  dependencies: thread_dep,
)

# GMock library
gmock_lib = static_library(
  'gmock',
  'googlemock/src/gmock-all.cc',
  include_directories: [gmock_inc, gmock_src_inc, gtest_inc, gtest_src_inc],
  link_with: gtest_lib,
  dependencies: thread_dep,
)

gmock_dep = declare_dependency(
  include_directories: [gmock_inc, gtest_inc],
  link_with: [gmock_lib, gtest_lib],
  dependencies: thread_dep,
)

# GMock main library
gmock_main_lib = static_library(
  'gmock_main',
  'googlemock/src/gmock_main.cc',
  include_directories: [gmock_inc, gmock_src_inc, gtest_inc, gtest_src_inc],
  link_with: [gmock_lib, gtest_lib],
  dependencies: thread_dep,
)

gmock_main_dep = declare_dependency(
  include_directories: [gmock_inc, gtest_inc],
  link_with: [gmock_lib, gmock_main_lib, gtest_lib],
  dependencies: thread_dep,
)

meson.override_dependency('gtest', gtest_dep)
meson.override_dependency('gtest_main', gtest_main_dep)
meson.override_dependency('gmock', gmock_dep)
meson.override_dependency('gmock_main', gmock_main_dep)

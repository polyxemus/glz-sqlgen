project('sqlgen', 'cpp',
  version: '0.4.0',
  default_options: [
    'cpp_std=c++20',
  ],
)

# sqlgen is a header-only template library
sqlgen_inc = include_directories('include')

# Dependencies - sqlgen uses reflect-cpp but we'll make it optional since
# we're creating a glaze-based version
reflect_dep = dependency('reflect-cpp', required: false, fallback: [])

# Main sqlgen dependency (header-only)
sqlgen_dep = declare_dependency(
  include_directories: sqlgen_inc,
)

meson.override_dependency('sqlgen', sqlgen_dep)

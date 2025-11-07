project('glaze', 'cpp',
  version: '6.0.2',
  default_options: [
    'cpp_std=c++20',
  ],
)

glaze_inc = include_directories('include', is_system: true)

glaze_glaze_dep = declare_dependency(
  include_directories: glaze_inc,
)

meson.override_dependency('glaze', glaze_glaze_dep)

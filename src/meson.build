
build_options = cc_id == 'gcc' ? ['c_std=gnu17', 'optimization=1'] : ['optimization=1']

exe = executable(
                 meson.project_name(),
                 'triangle.c',
                 link_language : 'c',
                 install: true,
                 override_options: build_options
                )

release = configuration_data()
release.set_quoted('TRIANGLE_RELEASE_VERSION', meson.project_version())
release_header = configure_file(output: 'triangle_release.h',
                                configuration: release)

# library: shared or static from the default_library option
lib = library(
              meson.project_name(),
              'triangle.c',
              'triangle_status.c',
              release_header,
              c_args: ['-DTRILIBRARY', '-DANSI_DECLARATORS'],
              name_prefix: 'lib',
              vs_module_defs: 'triangle.def',
              install: true,
              override_options: build_options
             )

install_headers('triangle.h', 'triangle_status.h')

triangle_dep = declare_dependency(
                                  link_with: lib,
                                  include_directories: include_directories('.')
                                 )
meson.override_dependency(meson.project_name(), triangle_dep)

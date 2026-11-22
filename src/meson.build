
build_options = cc_id == 'gcc' ? ['c_std=gnu17', 'optimization=1'] : ['optimization=1']

# install only when built on its own, not as a subproject
install = not meson.is_subproject()

exe = executable(
                 meson.project_name(),
                 'triangle.c',
                 link_language : 'c',
                 dependencies: m_dep,
                 install: install,
                 override_options: build_options
                )

release = configuration_data()
release.set_quoted('TRIANGLE_RELEASE_VERSION', meson.project_version())
release_header = configure_file(output: 'triangle_release.h',
                                configuration: release)

# export only the API from the shared library; Windows uses triangle.def
export_args = []
export_files = []
if host_machine.system() == 'darwin'
  export_args = ['-Wl,-exported_symbols_list,' + meson.current_source_dir() / 'triangle.exports']
  export_files = ['triangle.exports']
elif host_machine.system() != 'windows'
  export_args = ['-Wl,--version-script=' + meson.current_source_dir() / 'triangle.map']
  export_files = ['triangle.map']
endif

# library: shared or static from the default_library option
lib = library(
              meson.project_name(),
              'triangle.c',
              'triangle_status.c',
              release_header,
              c_args: ['-DTRILIBRARY', '-DANSI_DECLARATORS'],
              dependencies: m_dep,
              name_prefix: 'lib',
              vs_module_defs: 'triangle.def',
              link_args: export_args,
              link_depends: export_files,
              install: install,
              override_options: build_options
             )

if install
  install_headers('triangle.h', 'triangle_status.h')
endif

triangle_dep = declare_dependency(
                                  link_with: lib,
                                  dependencies: m_dep,
                                  include_directories: include_directories('.')
                                 )
meson.override_dependency(meson.project_name(), triangle_dep)

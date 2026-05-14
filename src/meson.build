
exe = executable(
                 meson.project_name(),
                 'triangle.c',
                 link_language : 'c',
                 c_args: cc.get_supported_arguments([
                   '-ffp-contract=off',
                   '-fno-tree-vectorize',
                 ]),
                 override_options: ['optimization=2'],
                 install: true
                )


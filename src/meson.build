
exe = executable(
                 meson.project_name(),
                 'triangle.c',
                 link_language : 'c',
                 install: true,
                 override_options: cc_id == 'gcc' ? ['c_std=gnu17', 'optimization=1'] : ['optimization=1']
                )


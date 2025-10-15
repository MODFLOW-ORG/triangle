project(
  'triangle',
  ['c'],
  version: '1.6',
  license: 'Copyright 1993, 1995, 1997, 1998, 2002, 2005 Jonathan Richard Shewchuk',
  meson_version: '>= 1.3.1',
  default_options : [
    'b_vscrt=static_from_buildtype', # Link runtime libraries statically on Windows
    'buildtype=release',
])

cc = meson.get_compiler('c')
cc_id = cc.get_id()
message('Compiler ID:', cc_id)

compile_args = []
link_args = []

if cc_id == 'gcc'
  link_args += ['-lm']
endif

if cc_id == 'intel'
  compile_args += ['-diag-disable=10441']
  link_args += ['-diag-disable=10441']
endif

system = build_machine.system()
if system == 'linux'
  compile_args += '-DLINUX'
elif system == 'darwin'
  compile_args += '-DAPPLE'
elif system == 'windows'
  compile_args += ['-DWIN32', '-DNO_TIMER']
endif

add_project_arguments(cc.get_supported_arguments(compile_args), language: 'c')
add_project_link_arguments(cc.get_supported_arguments(link_args), language: 'c')

subdir('src')

# tests
test('help', 
     exe,
     args : ['-h'],
     workdir: meson.current_source_dir()/'test')

test('sample', 
     exe,
     args : ['-p', 'A'],
     workdir: meson.current_source_dir()/'test')


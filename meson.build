project(
  'triangle',
  ['c'],
  version: '1.6.2',
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
  compile_args += ['-fexcess-precision=standard', '-ffp-contract=off']
  link_args += ['-lm']
endif

if cc_id == 'intel'
  compile_args += ['-diag-disable=10441', '-fp-model=strict']
  link_args += ['-diag-disable=10441']
endif

if cc_id == 'intel-llvm'
  compile_args += ['-fp-model=strict']
endif

# LINUX and CPU86 set the x87 control word, so only on x86
system = host_machine.system()
x86 = host_machine.cpu_family() in ['x86', 'x86_64']
if system == 'linux'
  if x86
    compile_args += '-DLINUX'
  endif
elif system == 'darwin'
  compile_args += '-DAPPLE'
elif system == 'windows'
  compile_args += ['-DNO_TIMER', '-DWINDOWS']
  if x86
    compile_args += '-DCPU86'
  endif
endif

m_dep = cc.find_library('m', required: false)

add_project_arguments(cc.get_supported_arguments(compile_args), language: 'c')
add_project_link_arguments(cc.get_supported_arguments(link_args), language: 'c')

subdir('src')

# tests, unless triangle is a subproject
if meson.is_subproject()
  subdir_done()
endif

test('help', 
     exe,
     args : ['-h'],
     workdir: meson.current_source_dir()/'test')

test('sample', 
     exe,
     args : ['-p', 'A.poly'],
     workdir: meson.current_source_dir()/'test')

test('library',
     executable('test_library',
                'test/test_library.c',
                dependencies: [triangle_dep, m_dep]))

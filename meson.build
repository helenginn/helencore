project('helencore', 'cpp', 'c')
boost_dep = dependency('boost')
arg_list = []

if (host_machine.system() == 'darwin')
  arg_list += '-mmacosx-version-min=10.14'
endif

helencore_scs = false

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Run the version and hash generator
hash_script = find_program('get_hash.sh')

# This will produce the dependency file.
run_command('get_hash.sh')

helencore = library('helencore',
'hcsrc/libica/svdcmp.cpp',
'hcsrc/libica/matrix.cpp',
'hcsrc/lbfgs.c',
'hcsrc/Converter.cpp',
'hcsrc/Canonical.cpp',
'hcsrc/Fibonacci.cpp',
'hcsrc/FileReader.cpp',
'hcsrc/mat3x3.cpp',
'hcsrc/mat4x4.cpp',
'hcsrc/maths.cpp',
'hcsrc/Matrix.cpp',
'hcsrc/RefinementGridSearch.cpp', 
'hcsrc/RefinementLBFGS.cpp', 
'hcsrc/RefinementList.cpp', 
'hcsrc/RefinementNelderMead.cpp', 
'hcsrc/RefinementStepSearch.cpp', 
'hcsrc/RefinementStrategy.cpp', 
'hcsrc/Timer.cpp', 
'hcsrc/vec3.cpp',
link_args: arg_list,
cpp_args: arg_list, dependencies : [ boost_dep ], install: true)

install_headers([
'hcsrc/Any.h',
'hcsrc/Blast.h',
'hcsrc/Converter.h',
'hcsrc/Canonical.h',
'hcsrc/Fibonacci.h',
'hcsrc/FileReader.h',
'hcsrc/font.h',
'hcsrc/lbfgs.h',
'hcsrc/RefineMat3x3.h',
'hcsrc/RefinementGridSearch.h',
'hcsrc/RefinementLBFGS.h',
'hcsrc/RefinementList.h',
'hcsrc/RefinementNelderMead.h',
'hcsrc/RefinementStepSearch.h',
'hcsrc/RefinementStrategy.h',
'hcsrc/font.h',
'hcsrc/charmanip.h',
'hcsrc/maths.h',
'hcsrc/mat3x3.h',
'hcsrc/mat4x4.h',
'hcsrc/Matrix.h',
'hcsrc/Timer.h', 
'hcsrc/vec3.h',
],
 subdir : 'hcsrc')

pkg = import('pkgconfig')
h = ['helencore'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate([helencore],
             subdirs : [''],
             version : '1.0',
             name : 'helencore',
             filebase : 'helencore',
             description : 'Library for 3D utilities by Helen')

helencore_inc = include_directories('hcsrc')
helencore_subdep = declare_dependency(link_with: helencore, include_directories: helencore_inc)

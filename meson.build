project('VoxelThingYeah', 'cpp',
	default_options: ['cpp_standard=c++11', 'warning_level=2'],
	version: '0.0.0')

sources = [
	'src/main.cpp',
	'src/vector.cpp'
]
includes = [
]

args = [
	'-Wall', 
	'-Wextra', 
	'-pedantic', 
	'-Wno-unused-parameter', 
	'-Wno-missing-field-initializers', 
	'-Wno-c++11-narrowing',
	'-fno-exceptions'
]
add_global_arguments(args, language: 'cpp')

# for our cmake dependencies
cmake = import('cmake')

bgfx = cmake.subproject('bgfx.cmake')

deps = [
	bgfx.dependency('bimg'), bgfx.dependency('astc'), bgfx.dependency('astc-codec'), bgfx.dependency('bx'), bgfx.dependency('bgfx'),
	dependency('sdl2',
		required: true,
		static: true,
		method: 'config-tool')
]

if host_machine.system() != 'windows'
	deps += [meson.get_compiler('cpp').find_library('dl')]
endif

executable(meson.project_name(), sources,
	include_directories: includes,
	dependencies: deps
)
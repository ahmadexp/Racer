project('Racer', 'cpp', version: '0.1', default_options: ['cpp_std=c++11'])

cpp = meson.get_compiler('cpp')

# Platform detection
system = host_machine.system()

# Common Args
add_project_arguments('-D_THREAD_SAFE', language: 'cpp')
add_project_arguments('-Wno-write-strings', language: 'cpp')

# Include Directories
inc = include_directories(
    'source',
    'source/generic',
    'source/projectspecific',
    'source/include'
)

# Dependencies List
deps = []

if system == 'windows'
    add_project_arguments('-D_WIN32', language: 'cpp')
    
    # On Windows, use local libraries in source/lib
    lib_dir = meson.current_source_dir() + '/source/lib'
    
    # Find local libs
    sdl_lib = cpp.find_library('SDL', dirs: lib_dir, required: true)
    sdlmain_lib = cpp.find_library('SDLmain', dirs: lib_dir, required: true)
    # sdlimage_lib = cpp.find_library('SDL_image', dirs: lib_dir, required: true) # Skipped, using lodepng
    sdlmixer_lib = cpp.find_library('SDL_mixer', dirs: lib_dir, required: true)
    
    deps += [sdl_lib, sdlmain_lib, sdlmixer_lib]
    
    deps += dependency('gl')
    deps += dependency('glu')

elif system == 'darwin'
    add_project_arguments('-D__MACOSX__', language: 'cpp')
    add_project_arguments('-DGL_SILENCE_DEPRECATION', language: 'cpp')
    
    # MacOS dependencies
    # Prefer sdl12_compat or sdl (legacy)
    sdl_dep = dependency('sdl12_compat', required: false)
    if not sdl_dep.found()
        sdl_dep = dependency('sdl', required: true)
    endif
    deps += sdl_dep
    
    # sdl_lib = cpp.find_library('SDL', dirs: ['/usr/local/lib'], required: true)
    # deps += sdl_lib
    
    # SDL_image removed - using lodepng in-tree
    
    # SDL_mixer removed - unused and causing conflicts
    # sdl_mixer_dep = dependency('SDL_mixer', required: false)
    # if not sdl_mixer_dep.found()
    #      sdl_mixer_dep = dependency('SDL2_mixer', required: false)
    # endif
    # if sdl_mixer_dep.found()
    #     deps += sdl_mixer_dep
    # endif
    
    sdl_net_dep = dependency('SDL_net', required: false)
    if sdl_net_dep.found()
        deps += sdl_net_dep
    endif
    
    # Use Apple OpenGL Framework on macOS
    deps += dependency('appleframeworks', modules: ['OpenGL', 'Cocoa', 'IOKit', 'Carbon'])
    # deps += dependency('gl')
    # deps += dependency('glu')

else
    # Linux and others
    add_project_arguments('-D__LINUX__', language: 'cpp')
    
    sdl_dep = dependency('sdl', required: false)
    if not sdl_dep.found()
         sdl_dep = dependency('sdl12_compat', required: true)
    endif
    deps += sdl_dep
    
    deps += dependency('SDL_mixer', required: true)
    
    sdl_net_dep = dependency('SDL_net', required: false)
    if sdl_net_dep.found()
        deps += sdl_net_dep
    endif

    deps += dependency('gl')
    deps += dependency('glu')
endif

executable('Racer',
    'source/main.cpp',
    include_directories: inc,
    dependencies: deps,
    install: true
)

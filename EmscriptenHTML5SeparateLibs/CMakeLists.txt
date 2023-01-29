cmake_minimum_required(VERSION 3.0.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE for Emscripten must be defined!")
endif()

if(GENERATE_BUILD_ONLY_TARGETS)
	set(CMAKE_CONFIGURATION_TYPES
		"WASM-Release"
		"WASM-RelWithDebInfo"
		"WASM-Debug"
	CACHE STRING "" FORCE)

	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE WASM-Release CACHE STRING "Build type: WASM-Debug, WASM-Release or WASM-RelWithDebInfo" FORCE)
	endif()
	
	set(CMAKE_EXE_LINKER_FLAGS_WASM-RELEASE "-O3")
	set(CMAKE_EXE_LINKER_FLAGS_WASM-RELWITHDEBINFO "-O1 -g --emrun")
	set(CMAKE_EXE_LINKER_FLAGS_WASM-DEBUG "-O0 -g --emrun")
	set(CMAKE_C_FLAGS_WASM-DEBUG " ${CMAKE_C_FLAGS_WASM-DEBUG} -O0 -gsource-map -g")
endif()

include(${CMAKE_TOOLCHAIN_FILE})
project(EmscriptenHTML5SeparateLibs)

set(SOURCE_FILES window.cpp callbacks.cpp main.cpp) 
set(RENDERER_FILES renderer.cpp renderer.h)

add_library(Renderer SHARED
	${RENDERER_FILES}
) 

add_executable(EmscriptenHTML5SeparateLibs ${SOURCE_FILES})

set_target_properties(Renderer PROPERTIES 
	COMPILE_FLAGS "-fPIC"
    LINK_FLAGS "-sSIDE_MODULE=1 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY --no-entry -fPIC"
    SUFFIX ".wasm"
)

set_target_properties(EmscriptenHTML5SeparateLibs PROPERTIES 
	COMPILE_FLAGS "-fPIC"
    LINK_FLAGS "-sMAIN_MODULE=1 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY -fPIC" 
    SUFFIX ".html"
)
	
add_dependencies(EmscriptenHTML5SeparateLibs Renderer)
target_link_libraries(EmscriptenHTML5SeparateLibs Renderer)
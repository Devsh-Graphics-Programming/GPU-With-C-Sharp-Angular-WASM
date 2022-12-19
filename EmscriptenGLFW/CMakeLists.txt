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
	set(CMAKE_EXE_LINKER_FLAGS_WASM-RELWITHDEBINFO "-O1 -g --emrun -gsplit-dwarf -gseparate-dwarf=DWARF_debug_symbols.dbg.wasm -gdwarf-5")
	set(CMAKE_EXE_LINKER_FLAGS_WASM-DEBUG "-O0 -g --emrun -gdwarf-5 -gsplit-dwarf -gseparate-dwarf=DWARF_debug_symbols.dbg.wasm")
endif()

include(${CMAKE_TOOLCHAIN_FILE})
project(EmscriptenGLFW)

file(GLOB SOURCE_FILES *.cpp) 
file(GLOB HEADER_FILES *.h *.hpp) 
add_executable(EmscriptenGLFW ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(EmscriptenGLFW PROPERTIES 
    LINK_FLAGS "-sUSE_WEBGL2=1 -sUSE_GLFW=3 -sFULL_ES3=1 -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY"
    SUFFIX ".html"
)
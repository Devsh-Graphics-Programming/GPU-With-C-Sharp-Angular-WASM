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
	set(CMAKE_EXE_LINKER_FLAGS_WASM-RELWITHDEBINFO "-O1 -g -gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm --emrun")
	set(CMAKE_EXE_LINKER_FLAGS_WASM-DEBUG "-O0 -g -gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm --emrun")
endif()

include(${CMAKE_TOOLCHAIN_FILE})
project(EmscriptenHTML5)

file(GLOB SOURCE_FILES *.cpp) 
file(GLOB HEADER_FILES *.h *.hpp) 
add_executable(EmscriptenHTML5 ${SOURCE_FILES} ${HEADER_FILES})

set(WASM_64MEMORY_MODE 0 CACHE STRING "0 for 32bit, 1 for full 64, 2 for 64bit clang compilation and 32bit wasm runtime workaround")
if("${WASM_64MEMORY_MODE}" STREQUAL "0" OR "${WASM_64MEMORY_MODE}" STREQUAL "1" OR "${WASM_64MEMORY_MODE}" STREQUAL "2")
	set_target_properties(EmscriptenHTML5 PROPERTIES 
		LINK_FLAGS "-sUSE_WEBGL2=1 -sFULL_ES3=1 -sALLOW-MEMORY-GROWTH -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY -sMEMORY64=${WASM_64MEMORY_MODE}"
		SUFFIX ".html")
else()
	message(FATAL_ERROR "Incorrect WASM_64MEMORY_MODE mode. Use 0, 1 or 2")
endif()
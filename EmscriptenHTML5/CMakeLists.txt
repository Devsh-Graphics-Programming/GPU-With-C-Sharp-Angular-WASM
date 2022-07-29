cmake_minimum_required(VERSION 3.0.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE for Emscripten must be defined!")
endif()

if(NOT WIN32)
	set(CMAKE_CONFIGURATION_TYPES
		"WASM-Debug"
		"WASM-Release"
		CACHE STRING "" FORCE
	)

	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE WASM-Release CACHE STRING "Build type: WASM-Debug or WASM-Release" FORCE)
	endif()
endif()

include(${CMAKE_TOOLCHAIN_FILE})
project(EmscriptenHTML5)

if(NOT CMAKE_HOST_WIN32)
	set(CMAKE_EXE_LINKER_FLAGS_WASM-RELEASE " ${CMAKE_EXE_LINKER_FLAGS_WASM-RELEASE} -O3")
	set(CMAKE_C_FLAGS_WASM-DEBUG " ${CMAKE_C_FLAGS_WASM-DEBUG} -O0 -gsource-map -g --emrun -gsplit-dwarf -gdwarf-5")
	set(CMAKE_EXE_LINKER_FLAGS_WASM-DEBUG "${CMAKE_EXE_LINKER_FLAGS_WASM-DEBUG} -O0 -gsource-map -g --emrun -gsplit-dwarf -gdwarf-5")
endif()

file(GLOB SOURCE_FILES *.cpp) 
file(GLOB HEADER_FILES *.h *.hpp) 
add_executable(EmscriptenHTML5 ${SOURCE_FILES} ${HEADER_FILES})

if(NOT CMAKE_HOST_WIN32)
	cmake_path(GET CMAKE_TOOLCHAIN_FILE PARENT_PATH EMSCRIPTEN_TOOLCHAIN_DIRECTORY)
	get_filename_component(EMSCRIPTEN_SDK_ROOT_PATH "${EMSCRIPTEN_TOOLCHAIN_DIRECTORY}/../../../../../" ABSOLUTE)

	add_custom_target(set_emscripten_environment 
		COMMAND chmod a+rwx ./emsdk_env.sh
		COMMAND ./emsdk_env.sh
		WORKING_DIRECTORY "${EMSCRIPTEN_SDK_ROOT_PATH}"
	)
	
	add_dependencies(EmscriptenGLFW set_emscripten_environment)
endif()

set_target_properties(EmscriptenHTML5 PROPERTIES 
    LINK_FLAGS "-sUSE_WEBGL2=1 -sFULL_ES3=1 -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY"
    SUFFIX ".html"
)
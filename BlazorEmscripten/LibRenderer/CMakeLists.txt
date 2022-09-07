cmake_minimum_required(VERSION 3.0.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE for Emscripten must be defined!")
endif()


include(${CMAKE_TOOLCHAIN_FILE})
project(LibRenderer)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g --emrun -gdwarf-5")

file(GLOB SOURCE_FILES *.cpp) 
file(GLOB HEADER_FILES *.h *.hpp) 
add_executable(BlazorNativeLib ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(BlazorNativeLib PROPERTIES 
    COMPILE_FLAGS "-fPIC"
    LINK_FLAGS "-sUSE_WEBGL2=1 -sFULL_ES3=1 -sALLOW-MEMORY-GROWTH -sWASM=1 -sWASM_BIGINT -sMIN_WEBGL_VERSION=2 -sASYNCIFY -sMEMORY64=0 -sSIDE_MODULE=1 --no-entry -fPIC"
    SUFFIX ".wasm")
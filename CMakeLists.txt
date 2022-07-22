cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(GPU-With-C-Sharp-Angular-WASM)

set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    "RelWithDebInfo"
    CACHE STRING "" FORCE
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug, Release or RelWithDebInfo" FORCE)
endif()

option(UPDATE_SUBMODULES OFF "")

get_filename_component(RELEX_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
get_filename_component(RELEX_THIRD_PARTY_PATH "${RELEX_ROOT_PATH}/3rdparty" ABSOLUTE)
get_filename_component(RELEX_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}" ABSOLUTE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

include(ExternalProject)
find_package(Git REQUIRED)

if(UPDATE_SUBMODULES)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${RELEX_ROOT_PATH} submodule update --init --recursive)
endif()

add_subdirectory(3rdparty 3rdparty)

if(WIN32)
	message(FATAL_ERROR "we don't support top root builds for windows OS") # there is no time to patch all OS-builds for now
else()
	set(EMSCRIPTEN_GLFW_CONFIG $<IF:$<CONFIG:Debug>,WASM-Debug,WASM-Release>)
	set(EMSCRIPTEN_GLFW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenGLFW")
	set(EMSCRIPTEN_GLFW_BINARY_DIR "${EMSCRIPTEN_GLFW_SOURCE_DIR}/build")
	
	list(APPEND EMSCRIPTEN_GLFW_CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${EMSCRIPTEN_GLFW_CONFIG}") 
	list(APPEND EMSCRIPTEN_GLFW_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${EMSCRIPTEN_PLATFORM_DIRECTORY}/Emscripten.cmake") 
	
	ExternalProject_Add(EmscriptenGLFW
		DOWNLOAD_COMMAND  ""
		SOURCE_DIR        "${EMSCRIPTEN_GLFW_SOURCE_DIR}"
		BINARY_DIR        "${EMSCRIPTEN_GLFW_BINARY_DIR}"
		CMAKE_ARGS        ${EMSCRIPTEN_GLFW_CMAKE_ARGS}
		BUILD_COMMAND     ${CMAKE_COMMAND} -E echo "Starting ${EMSCRIPTEN_GLFW_CONFIG} build"
		COMMAND           ${CMAKE_COMMAND} --build ${EMSCRIPTEN_GLFW_BINARY_DIR} --config ${EMSCRIPTEN_GLFW_CONFIG} -v
		INSTALL_COMMAND   ""
		TEST_COMMAND      ""
	)
	
	add_dependencies(EmscriptenGLFW build_emscripten)
endif()
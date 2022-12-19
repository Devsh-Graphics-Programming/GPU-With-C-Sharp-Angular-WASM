cmake_minimum_required(VERSION 3.24)

set(CMAKE_CONFIGURATION_TYPES
	Release
	RelWithDebInfo
	Debug
)

option(GENERATE_BUILD_ONLY_TARGETS "" ON)

project(GPU-With-C-Sharp-Angular-WASM)

set(_PROJECTS_BUILD_DIRECTORY_ "${CMAKE_CURRENT_BINARY_DIR}/projects" CACHE INTERNAL "")

add_subdirectory(3rdparty 3rdparty)

ADD_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenHTML5/CMakeSettings.json.cmake")
ADD_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenGLFW/CMakeSettings.json.cmake")

# EmscriptenHTML5 project
#set(_THIS_PROJECT_BUILD_DIRECTORY_ "${_PROJECTS_BUILD_DIRECTORY_}/EmscriptenHTML5")
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenHTML5/CMakeSettings.json.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenHTML5/CMakeSettings.json")
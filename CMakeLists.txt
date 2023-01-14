cmake_minimum_required(VERSION 3.24)

set(SERVE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")

set(CMAKE_CONFIGURATION_TYPES
	Release
	RelWithDebInfo
	Debug
)

option(GENERATE_BUILD_ONLY_TARGETS "" ON)

project(GPU-With-C-Sharp-Angular-WASM)

set(_PROJECTS_BUILD_DIRECTORY_ "${CMAKE_CURRENT_BINARY_DIR}/projects" CACHE INTERNAL "")

# Emscripten settings - match Blazor's
set(FORCE_NET_EMSCRIPTEN_MATCH ON CACHE BOOL "")
set(FORCE_NET_EMSCRIPTEN_VERSION "2.0.23" CACHE INTERNAL "")
set(FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION "6.0.12" CACHE INTERNAL "")

add_subdirectory(3rdparty 3rdparty)

# Custom Emscripten projects
ADD_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenHTML5/CMakeSettings.json.cmake")
ADD_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenHTML5SeparateLibs/CMakeSettings.json.cmake")
ADD_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenGLFW/CMakeSettings.json.cmake")

# Blazor Emscripten projects
ADD_BLAZOR_EMSCRIPTEN_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/BlazorEmscripten")

# Native projects
ADD_NATIVE_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/Native" ON)
#ADD_NATIVE_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/NativeCSharp" OFF)

execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/data"
				COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_BINARY_DIR}/data"
)
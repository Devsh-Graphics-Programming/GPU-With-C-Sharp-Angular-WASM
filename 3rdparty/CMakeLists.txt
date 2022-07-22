cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(relexDeps)

get_filename_component(RELEX_DEPS_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
get_filename_component(RELEX_DEPS_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}" ABSOLUTE)

set(EMSCRIPTEN_PLATFORM_DIRECTORY "${RELEX_THIRD_PARTY_PATH}/emsdk/upstream/emscripten/cmake/Modules/Platform" CACHE INTERNAL "")
set(EMSCRIPTEN_OUTPUT "${EMSCRIPTEN_PLATFORM_DIRECTORY}/Emscripten.cmake" PARENT_SCOPE)

add_custom_command(OUTPUT "${EMSCRIPTEN_OUTPUT}"
	COMMAND ./emsdk install latest
	COMMAND ./emsdk activate latest
	WORKING_DIRECTORY ${RELEX_DEPS_ROOT_PATH}/emsdk
	COMMENT "Getting and updating emscripten sdk tools..."
	COMMAND_EXPAND_LISTS
	VERBATIM
)

add_custom_target(build_emscripten ALL
	DEPENDS ${EMSCRIPTEN_OUTPUT}
)
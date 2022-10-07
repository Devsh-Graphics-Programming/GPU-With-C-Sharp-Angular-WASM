# LLVM Clang with reflection

set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "") # for Clang when building with generators which don't support multi-config builds
set(LLVM_ENABLE_PROJECTS "lld;clang" CACHE STRING "") 
set(LLVM_TARGETS_TO_BUILD "host;WebAssembly" CACHE STRING "")

set(LLVM_INCLUDE_EXAMPLES OFF CACHE INTERNAL "") 
set(LLVM_INCLUDE_TESTS OFF CACHE INTERNAL "")

add_subdirectory(llvm/llvm llvm)
set(LLVM_ROOT "${CMAKE_CURRENT_BINARY_DIR}/llvm/$<CONFIG>/bin" CACHE INTERNAL "") # for .emscripten file

unset(CMAKE_BUILD_TYPE CACHE)
unset(LLVM_INCLUDE_EXAMPLES CACHE)
unset(LLVM_INCLUDE_TESTS CACHE)

# Emscripten

get_filename_component(_ORIGINAL_SOURCE_ "${CMAKE_CURRENT_SOURCE_DIR}/emsdk" ABSOLUTE)
get_filename_component(_COPY_BIN_SOURCE_ "${CMAKE_CURRENT_BINARY_DIR}/emscripten" ABSOLUTE)

set(EMSCRIPTEN_TOOLCHAIN_FILE "${_COPY_BIN_SOURCE_}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" CACHE INTERNAL "")

add_custom_target(emscripten_clean
	COMMAND "${CMAKE_COMMAND}" -E rm -Rf "${_COPY_BIN_SOURCE_}"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${_ORIGINAL_SOURCE_}" "${_COPY_BIN_SOURCE_}" 
	COMMENT "Cleaning Emscripten..."
)

add_custom_command(
	OUTPUT "${_COPY_BIN_SOURCE_}/.emscripten"
	COMMAND emsdk.bat install latest
	COMMAND emsdk.bat activate latest
	COMMAND "${CMAKE_COMMAND}" "-DLLVM_ROOT=${LLVM_ROOT}" "-DEMSCRIPTEN_FILE=${_COPY_BIN_SOURCE_}/.emscripten" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscriptenPatch.cmake"
	WORKING_DIRECTORY "${_COPY_BIN_SOURCE_}"
	COMMENT "Installing and activating Emscripten with custom LLVM..."
	VERBATIM
)

add_custom_target(emscripten_build DEPENDS "${_COPY_BIN_SOURCE_}/.emscripten")
add_dependencies(emscripten_build emscripten_clean)

unset(_ORIGINAL_SOURCE_)
unset(_COPY_BIN_SOURCE_)
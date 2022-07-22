# - Find Emscripten
#
# EMSCRIPTEN_TOOLCHAIN_FILE - Absolute path to Emscripten toolchain file
# EMSCRIPTEN_FOUND - True if Emscripten found.

# Look for the toolchain
find_file(EMSCRIPTEN_TOOLCHAIN_FILE NAMES Emscripten.cmake HINTS ${EMSCRIPTEN_PLATFORM_DIRECTORY})

# Handle the QUIETLY and REQUIRED arguments and set EMSCRIPTEN_FOUND to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Emscripten DEFAULT_MSG EMSCRIPTEN_TOOLCHAIN_FILE)

if(EMSCRIPTEN_FOUND)
	set(EMSCRIPTEN_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN_FILE})
else()
	set(EMSCRIPTEN_TOOLCHAIN_FILE)
endif()

mark_as_advanced(EMSCRIPTEN_TOOLCHAIN_FILE)

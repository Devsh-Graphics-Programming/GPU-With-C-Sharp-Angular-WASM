if(NOT DEFINED LLVM_ROOT)
	message(FATAL_ERROR  "'LLVM_ROOT' variable must be defined for this patch-script!")
endif()

if(NOT DEFINED EMSCRIPTEN_FILE)
	message(FATAL_ERROR  "'EMSCRIPTEN_FILE' variable must be defined for this patch-script!")
endif()

cmake_path(GET EMSCRIPTEN_FILE PARENT_PATH EMSCRIPTEN_FILE_DIRECTORY)
cmake_path(CONVERT "${EMSCRIPTEN_FILE_DIRECTORY}" TO_CMAKE_PATH_LIST _EMSCRIPTEN_FILE_DIRECTORY_)
cmake_path(CONVERT "${LLVM_ROOT}" TO_CMAKE_PATH_LIST _LLVM_ROOT_)

file(READ "${EMSCRIPTEN_FILE}" _EMSCRIPTEN_FILE_CONTENT_)
string(APPEND _EMSCRIPTEN_FILE_CONTENT_ "\n# LLVM_ROOT is overriden in order to use Clang with reflection\n")
string(APPEND _EMSCRIPTEN_FILE_CONTENT_ "LLVM_ROOT = '${_LLVM_ROOT_}'")
file(WRITE "${_EMSCRIPTEN_FILE_DIRECTORY_}/.emscripten" "${_EMSCRIPTEN_FILE_CONTENT_}")
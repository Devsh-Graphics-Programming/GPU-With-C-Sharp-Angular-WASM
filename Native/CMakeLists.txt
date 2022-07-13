cmake_minimum_required(VERSION 3.2)
include(${CMAKE_TOOLCHAIN_FILE})
project(NativeApplication)

set(SOURCE_FILES main.cpp renderer.cpp callbacks.cpp window.cpp)

add_executable(NativeApplication ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include_directories(${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
target_link_libraries(NativeApplication ${OPENGL_gl_LIBRARY} GLEW::GLEW glfw)

message(STATUS "Configuring Native Example complete")
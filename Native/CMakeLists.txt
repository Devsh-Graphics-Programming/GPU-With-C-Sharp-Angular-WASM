cmake_minimum_required(VERSION 3.23)
project(NativeApplication)

set(SOURCE_FILES main.cpp renderer.cpp callbacks.cpp window.cpp)
set(GLOB HEADER_FILES *.h)
add_executable(NativeApplication ${SOURCE_FILES} ${HEADER_FILES})

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(./3rdparty/glfw)

include_directories(${GLFW_INCLUDE_DIR})
target_link_libraries(NativeApplication ${OPENGL_gl_LIBRARY} glfw)

message(STATUS "Configuring Native Example complete")
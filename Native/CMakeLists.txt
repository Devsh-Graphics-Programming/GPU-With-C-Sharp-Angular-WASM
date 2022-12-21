cmake_minimum_required(VERSION 3.23)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

# 3rdparty libraries
add_subdirectory(3rdparty/glfw "${CMAKE_CURRENT_BINARY_DIR}/glfw" EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/GLEW/glew-2.1.0/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew" EXCLUDE_FROM_ALL)
find_package(OpenGL REQUIRED)

project(NativeApplication)

set(SOURCE_FILES main.cpp renderer.cpp callbacks.cpp window.cpp)
set(GLOB HEADER_FILES *.h)
add_executable(NativeApplication ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(NativeApplication PUBLIC
	${OPENGL_INCLUDE_DIRS}
	$<TARGET_PROPERTY:glew_s,INCLUDE_DIRECTORIES>
)
target_link_libraries(NativeApplication ${OPENGL_gl_LIBRARY} glew_s glfw)

message(STATUS "Configuring Native Example complete")
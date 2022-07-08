cmake_minimum_required(VERSION 3.2)
project(RelexPoC)

include_directories($ENV{GLEW_INCLUDE_DIR} $ENV{GLFW_INCLUDE_DIR})
link_directories($ENV{GLEW_LIBRARY} $ENV{GLFW_LIBRARY})

find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp)
add_executable(RelexPoC ${SOURCE_FILES})

target_link_libraries(RelexPoC ${OPENGL_gl_LIBRARY} glew32 glfw3dll)
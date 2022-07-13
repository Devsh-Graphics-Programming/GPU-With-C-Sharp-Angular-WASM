cmake_minimum_required(VERSION 3.23)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not defined, cmake will not be able to find vcpkg project. Use 'vcpkg integrate install' to obtain the filepath that should go in CMAKE_TOOLCHAIN_FILE")
endif()


set(BUILD_ANGULAR_BLAZOR FALSE CACHE BOOL "Do you want to build C# app with angular & blazor")
if(BUILD_ANGULAR_BLAZOR)
add_subdirectory(AngularBlazor)
endif()


set(BUILD_ANGULAR_BLAZOR_EMSCRIPTEN FALSE CACHE BOOL "Do you want to build a C++ example with angular & blazor compiled with emscript")
if(BUILD_ANGULAR_BLAZOR_EMSCRIPTEN)
add_subdirectory(AngularBlazorEmscripten)
endif()


set(BUILD_BLAZOR_HTML5 FALSE CACHE BOOL "C#,html5,blazor")
if(BUILD_BLAZOR_HTML5)
add_subdirectory(BlazorHTML5)
endif()


set(BUILD_EMSCRIPTEN_GLFW FALSE CACHE BOOL "C++,glfw,emscripten")
if(BUILD_EMSCRIPTEN_GLFW)
add_subdirectory(EmscriptenGLFW)
endif()


set(BUILD_EMSCRIPTEN_HTML5 FALSE CACHE BOOL "C++,html5,emscripten ")
if(BUILD_EMSCRIPTEN_HTML5)
add_subdirectory(EmscriptenHTML5)
endif()


set(BUILD_NATIVE FALSE CACHE BOOL "C++,glfw,msvc app that runs in a standalone window")
if(BUILD_NATIVE)
    add_subdirectory(Native)
endif()

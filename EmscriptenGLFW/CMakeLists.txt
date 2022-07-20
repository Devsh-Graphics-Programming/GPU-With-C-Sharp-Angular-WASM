CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
include(${CMAKE_TOOLCHAIN_FILE})


PROJECT(EmscriptenGLFW)


FILE(GLOB SOURCE_FILES *.cpp) 
FILE(GLOB HEADER_FILES *.h *.hpp) 
add_executable(EmscriptenGLFW ${SOURCE_FILES} ${HEADER_FILES})


#add defines here
#add_definitions( ) # TODO: shouldn't all the USE_WEBGL2 etc. go here?

set_target_properties(EmscriptenGLFW
PROPERTIES 
    LINK_FLAGS "-s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_GLFW=3 -s FULL_ES3=1 -s WASM=1 -sWASM_BIGINT -s MIN_WEBGL_VERSION=2 -s USE_GLFW=3 -sMIN_WEBGL_VERSION=2 -sASYNCIFY --bind"
    SUFFIX ".html"
)
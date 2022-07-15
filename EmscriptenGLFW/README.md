install emscripten via vcpkg

```
vcpkg install liblzma:wasm32-emscripten 
```


modify the function definition in `CMake\share\cmake-3.23\Modules\TestBigEndian.cmake` like this so it passes vcpkg install 

```
function(TEST_BIG_ENDIAN VARIABLE)
  if(";${CMAKE_C_BYTE_ORDER};${CMAKE_CXX_BYTE_ORDER};${CMAKE_CUDA_BYTE_ORDER};${CMAKE_OBJC_BYTE_ORDER};${CMAKE_OBJCXX_BYTE_ORDER};" MATCHES ";(BIG_ENDIAN|LITTLE_ENDIAN);")
    set(order "${CMAKE_MATCH_1}")
    if(order STREQUAL "BIG_ENDIAN")
      set("${VARIABLE}" 1 PARENT_SCOPE)
    else()
      set("${VARIABLE}" 0 PARENT_SCOPE)
    endif()
  elseif(DEFINED EMSCRIPTEN) 
    set("${VARIABLE}" 0 PARENT_SCOPE) # 0 because the machine uses little endian
  else()
    __TEST_BIG_ENDIAN_LEGACY_IMPL(is_big)
    set("${VARIABLE}" "${is_big}" PARENT_SCOPE)
  endif()
endfunction()
```

compile with 
emcc renderer.cpp window.cpp callbacks.cpp main.cpp -o index.html -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1 -s WASM=1

run server in the same dir as index.html
python -m http.server

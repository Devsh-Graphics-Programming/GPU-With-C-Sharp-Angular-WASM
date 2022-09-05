 
| App                   |  Debug   | Release |   RWDI  |
| ---                   |   ---    |   ---   |   ---   |
| Native C++            | 3101 kB  |  301 kB |   N/A   |
| Native C#             | 172 kB   |  157 kB |   N/A   |
| Emscripten GLFW       | 5662 kB  |  522 kB | 5294 kB |
| Emscripten HTML       | 5640 kb  |  510 kB | 5270 kB |
| Emscripten STATIC lib | 14268 kB | 5113 kB | 14987 kB|
| Emscripten SHARED lib | 14987 kB | 5097 kB | 14681 kB|
| Blazor Emscripten     |          |         |         |
 


- Native C++  
  - Debug executable is 705 kB, including 2396 kB .pdb file the total is 3101 kB .
  - Release executable is 301 kB, there is no extra files.
- Native C#
  - Excludes filesizes of dependencies and runtimes shipped with the app which take about 2700 kB
  - Debug executable is 145 kB, dll is 13 kB, pdb is 14 kB to a total of 172 kB
  - Release executable is 145 kB and dll is 12 kB, to a total of 157 kB
- Emscripten GLFW
  - Debug wasm module is 1505 kB, js is 341 kB, and separate DWARF is 3816 kB, to a total of 5662 kB 
  - Release wasm module is 382 kB, js is 140 kB, to a total of 522 kB
  - RelWithDebugInfo wasm module is 1260 kB, js is 308 kB and separate DWARF is 3726 kB to a total of 5294 kB
- Emscripten HTML5
  - Debug wasm module is 1506 kB, js is 315 kB, separate DWARF is 3819 kB, to a total of 5640 kB
  - Release wasm module is 382 kB, js is 128 kB, to a total of 510 kB
  - RelWithDebugInfo wasm module is 1260 kb, js is 283 kB, separate DWARF is 3727 kB to a total of 5270 kB
- Emscripten STATIC lib 
  - Debug wasm sidemodule with DWARF is 468 kB, but it's static linked into a 11486 kB main wasm module that also includes DWARF, js is 2782 kB. This adds up to  14268
  - Release wasm sidemodule is 151 kB but it's static linked into a 2205 kB main wasm module, js is 2908 kB, to a total of 5113 kB
  - RWDI wasm sidemodule with DWARF is 576 kB but it's static linked into a 11223 kB main wasm module that also includes DWARF, js is 3764 kB. This adds up to 14987 kB.
- Emscripten SHARED lib 
  - Debug wasm sidemodule with DWARF is 992 kB, main wasm module that also includes DWARF is 10551 kB, js is 2780 kB. This adds up to 14323 kB
  - Release wasm sidemodule is 164 kB main wasm module is 2036 kB, js is 2897 kB, to a total of 5097 kB
  - RWDI wasm sidemodule with DWARF is 567 kB, main wasm module that also includes DWARF is 10357 kB, js is 3757 kB. This adds up to 14681 kB
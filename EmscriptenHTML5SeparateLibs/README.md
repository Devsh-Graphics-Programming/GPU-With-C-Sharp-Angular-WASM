# GPU With C-Sharp Angular WASM
## C++ Compiled to WASM with Emscripten Split into Modules

<br>

## Summary

In this app, we separated Renderer.cpp from the rest of the app, compiled it, and then linked it back.

The renderer became Emscripten's SIDE_MODULE, and the main.cpp, window.cpp and callbacks.cpp became MAIN_MODULE.

We only cover implicit, runtime linking at loadtime, and static linking. Runtime linking at runtime is sufficiently problematic for WebAssembly.

<br>

# Installation
<br>

## Emscripten

Set up Emscripten as explained in readme in directory above.
Due to emscripten not supporting CMake's SHARED libraries, there needs to be a temporary workaround done in order to truly create a shared lib, instead of falling back to STATIC.
Navigate to `emsdk\upstream\emscripten\cmake\Modules\Platform`, and edit line #21 

```
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
```

to 

```
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
```

There is a [github issue](https://github.com/emscripten-core/emscripten/issues/17804) opened by us on emscripten-core/emscripten repository


<br>

## CMake

Do not use CMake gui if you plan on using Visual Studio 2022 as an IDE.

Instead, launch VS2022 and use the newly added option to open a CMake project by targetting this CMakeLists.txt

To change to static linking, change CMakeLists in the project from
```
add_library(Renderer SHARED
	${RENDERER_FILES}
) 
```
to
```
add_library(Renderer STATIC
	${RENDERER_FILES}
) 
```

<br>

## Building

In solution explorer on the top click on a button to switch between solutions and available views, then on CMake Target views and right click on EmscriptenHTML5 project to bring up a context menu that has an option to build. 
Another option is to change the launch target to `EmscriptenHTML5.html`, and then build by pressing `Ctrl + B`

<br>


# Launching

To properly launch this project, use the provided `run.py` script.
It starts a local server with its root in the directory of the repository, such that it can also view the contents of `GPU-With-C-Sharp-Angular-WASM/data/` which are obtained by the built example at runtime using wget.

<br>


# Debugging 

To debug the app, you need 
- Enabled DWARF in experimental chrome settings
- Installed DWARF extension for C++
- Built app in Debug configuration running on a server.	

Go to sources, place a breakpoint in JS. You can step into function calls until you reach C++.

You can add a FileSystem directory to the sources workspace, and place a breakpoint in C++ source code directly.



<br><br><br><br><br>

Notes:
-------

[Recommended chrome extension for debugging WASM](
https://chrome.google.com/webstore/detail/cc%20%20-devtools-support-dwa/pdcpmagijalfljmkmjngeonclgbbannb)

Both the executable (the main module) and the library (side module) need to be compiled with position independent code

Use `-gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm` to significantly decrease the size of the wasm file built in debug.

Do not use `-fdebug-compilation-dir` ever


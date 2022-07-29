# GPU With C-Sharp Angular WASM
## C++ Compiled to WASM with Emscripten

<br>

#Summary

In this app, we get rid of GLFW and replace it with HTML5.
The only things that are different compared to EmscriptenGLFW are:
- Callbacks
	- No longer uses glfw events 
	- Additional event that happens when mouse cursor is moved
- Window class
	- Obtains html object with id `"#canvas"`
	- Sets the canvas context as webgl's current
	- Assigns html callbacks to static methods/functions
	- Resizes html cavas 

<br>

# Installation
<br>

## Emscripten

Set up Emscripten as explained in readme in directory above

<br>

## CMake

Do not use CMake gui if you plan on using Visual Studio 2022 as an IDE.

Instead, launch VS2022 and use the newly added option to open a CMake project by targetting this CMakeLists.txt

<br>

## Building

In solution explorer on the top click on a button to switch between solutions and available views, then on CMake Target views and right click on EmscriptenHTML5 project to bring up a context menu that has an option to build. 
Another option is to change the launch target to `EmscriptenHTML5.html`, and then build by pressing `Ctrl + B`

<br>

-----

# Launching

To properly launch this project, use the provided `run.py` script.
It starts a local server with its root in the directory of the repository, such that it can also view the contents of `GPU-With-C-Sharp-Angular-WASM/data/` which are obtained by the built example at runtime using wget.



<br><br><br><br><br>

Notes:
-------

Recommended chrome extension for debugging
https://chrome.google.com/webstore/detail/cc%20%20-devtools-support-dwa/pdcpmagijalfljmkmjngeonclgbbannb
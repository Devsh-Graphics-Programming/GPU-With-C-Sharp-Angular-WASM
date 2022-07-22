# GPU With C-Sharp Angular WASM
## C++ Compiled to WASM with Emscripten

<br>

# Controls:
 - Use `1, 2, 3, 4, 5` on alphanumerical keyboard to switch background.
 - The fifth background is a cubemap, that can be toggled between two variants by repeatedly pressing the `5` key.
 - Click and drag with a mouse on screen to change the position of a camera.

<br>

# Installation
<br>

## Emscripten

To be able to compile this example, you need to install Emscripten and add it to path.
Instruction on how to install Emscripten can be found on the [official site](https://emscripten.org/docs/getting_started/downloads.html)

Once thats done and all the environment variables are assigned *(with --permament option, might require reboot)*, we can move on

<br>

## CMake

Do not use CMake gui if you plan on using Visual Studio 2022 as an IDE.

Instead, launch VS2022 and use the newly added option to open a CMake project by targetting this CMakeLists.txt

Then, assign a toolchain file:

- First way to do it is by adding an extra environment variable **EMSCRIPTEN**  with value ***path/to/emsdk***/**upstream/emscripten**
- Another way is to open CMakeSettings.json and edit `"cmakeToolchain"` to have a correct path

<br>

## Building

In solution explorer on the top click on a button to switch between solutions and available views, then on CMake Target views and right click on EmscriptenGLFW project to bring up a context menu that has an option to build. 
Another option is to change the launch target to `EmscriptenGLFW.html`, and then build by pressing `Ctrl + B`

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
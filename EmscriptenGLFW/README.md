# GPU With C-Sharp Angular WASM
### C++ to WebAssembly using Emscripten

To solve one problem at a time instead of dealing with all of them at once, in this variant of the app we only look into how to get the native app running in the browser.

We are going to utilize the fact that Emscripten comes with its own GLFW and GLEW function defintions that are spoofed into JavaScript function calls.

To continously render without making the browser unresponsive, we have to wait inbetween renderloop calls.
The main function changed to something like this 
```
int main() {
	w = new Window(1280, 720);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (w->keepAlive())
		w->update();
#endif

}
```

Emscriptens function to create the main loop is not much different from using JS' ` window.requestAnimationFrame(main_loop);` recursively.

`requestAnimationFrame` is apparently the recommended way of creating a render loops, however it seems that in conjunction with webassembly or DWARF debug format it can lead to stack smashing as more and more stack frames are added the longer the app is running.  



<br>

# Installation
<br>

### Emscripten

Set up Emscripten as explained in readme in directory above.

<br>

### CMake

Do not use CMake gui if you plan on using Visual Studio 2022 as an IDE. As of now, the custom toolchain passed into CMake gets disregarded if you use the generator `Visual Studio 17 2022` to configure a CMake project, and you use the default C++ compiler instead of `emcc`. 

Instead, launch VS2022 and use the newly added option to open a CMake project by targetting this CMakeLists.txt

![Opening a CMake project in VS2022](../Docs/img4.jpg)

<br>

## Building

In solution explorer on the top click on a button to switch between solutions and available views, then on CMake Target views and right click on EmscriptenGLFW project to bring up a context menu that has an option to build. 
Another option is to change the launch target to `EmscriptenGLFW.html`, and then build by pressing `Ctrl + B`

<br>

-----

# Launching

To properly launch this project, use the provided `run.py` script in the root directory of the repo.
```python run.py ./EmscriptenGLFW/```

It starts a local server with its root in the directory of the repository, such that it can also view the contents of `GPU-With-C-Sharp-Angular-WASM/data/` which are obtained by the built example at runtime using wget.


<br><br><br><br><br>


# Debugging

The debug info for this WebAssembly app is stored in DWARF format, that can be either embedded into the .wasm file, or be separated from it.

The DWARF format stores an absolute path to the source files on the computer.

To debug this app, you no longer use VS debugger or GDB, instead you use the chrome debugger.

While using DWARF, you cannot use source maps. -gsource-maps is now obsolete, yet despite that, if present, they will take precedence over the debug info from DWARF, and will end up scrambling names of locals.
One letdown of DWARF is that, unlike source maps, it does not list the source files in the sources tab of chrome debugger. The source files are still accessible though. 

Other than debugging C++, you can now also debug the generated JS glue code that loads the wasm app.

Disassembling the code yields reading from the generated  `EmscriptenGLFW.wasm`

<br><br><br><br><br>




Notes:
-------

Compiling with -g4 prints a warning that this option is outdated, and to use source maps, which are also outdated, but the latter is not displayed.
Do not use either.  
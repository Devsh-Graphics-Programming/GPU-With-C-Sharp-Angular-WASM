# C++ to WebAssembly using Emscripten

To solve one problem at a time instead of dealing with all of them at once, in this variant of the app we only look into how to compile the previously native app to WASM and get it running in the browser.

We are going to utilize the fact that Emscripten comes with its own GLFW and GLEW headers in the SDK.

## Findings

### Most of the C or C++ function definitions belonging to libraries in Emscripten's SDK are spoofed into JavaScript function calls

For example, there's no such thing as OpenGL ES 3.0 on the web, there's only a C header which uses Emscripten's JavaScript-Embedded-in-C macro to call WebGL 2 functions which are only usable from JavaScript.

Rest of the libraries, such as the standard C library, the Standard C++ Library, GLFW, SDL, etc. are bizarre hybrids where certain OS-specific API function calls are replaced with implementations that use JavaScript.

### You cannot use a "main loop"

If your JS or WASM function does not return within a given timeout, the web-page's responsiveness freezes and you will eventually get a prompt from the browser whether to kill or wait for the script.

Any rendering or similar continuous operation needs to be implemented via callbacks instead of loops.

### Faux Stack Smashing in Debug

Emscriptens function to create the main loop is not much different from enqueueing JavaScript's `window.requestAnimationFrame(main_loop);` recursively.

Calls to `requestAnimationFrame` from the callaback previously passed to `requestAnimationFrame` is apparently the recommended way of creating a render loops, however it seems that in conjunction with WASM or DWARF debug format it can lead to what appears like stack smashing, as more and more stack frames are seemingly added the longer the app is running.

However this is not true stack smashing as `requestAnimationFrame` merely enqueues the callback to be executed when the current one finishes.

### The Proper way to get Debugging Symbols

Over the period of 2018-2021 there were many compile options to generate debug info with Emscripten and ways to store and debug it, such as `-gsource-maps`, they should all be consideted outdated even and especially if coming from the Chromium Developer blog. The DWARF debug symbol generation is the only method which is actually understood by Chrome and does not lose your variable names or function names.

**While using DWARF, you cannot use source maps. -gsource-maps is now obsolete, yet despite that, if present, they will take precedence over the debug info from DWARF, and will end up scrambling names of locals.**

When the debug info for a WebAssembly app is stored in DWARF format, it can be either embedded into the .wasm file, or be separated from it. We tried both for this example and for exercise's sake we decided to go with the split DWARF.

The DWARF format stores an absolute path to the source files on the computer, _which means that its not going to load the sources properly when debugging if the sources move (such as when debugging on another machine to the building one)_. Unfortunately unlike a real IDE, Chrome Dev Tools won't ask you to locate a missing source.

_We tried to develop a utility for patching the paths for sources in the split DWARF, however we found out that the format is offset based, ergo the length of the path strings cannot be changed. Furthermore the paths get concatenated with directory paths which rules out exchanging paths for shorter paths and padding the remainder of the data with null characters._

One letdown of DWARF is that, unlike source maps, it does not list the source files in the sources tab of chrome debugger.

## Modifications Required

To continously render without causing the browser to become unresponsive, we had to change the main function to this: 
```cpp
Window* window;
void main_loop()
{
	window->update();
}

int main() {
	window = new Window(1280, 720);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (window->keepAlive())
		window->update();
#endif

}
```

## Building

As long as you have the Emscripten Target VS2022 component installed, the solution should load and work out-of-the-box.

The solution only has one project, build it by pressing `Ctrl + B` or straight up `F5` debugging it.

You can change the targets from Debug to RelWithDebInfo or Release.

## Debugging

**TL;DR All source-level CPU debugging features you're used to on Native, just work. For a discussion of GPU debugging using Renderdoc see the section in towards the bottom of the top level repository root README.md**

To debug this app, you can use either the Chrome Developer Tools Debugger once you run the example in Crhome or you can use the VS2022 debugger, which launches the example in Chrome and connects to the Chrome Developer Tools Debugger remotely.

Other than debugging C++, you can now also debug the generated JS glue code (a few thousand lines of it) that loads the wasm app and implements some of the functions that the Emscripten SDK declares in its C and C++ headers.

Disassembling the code yields reading from the generated `EmscriptenGLFW.wasm`


## Launching outside of the Debugger (TODO: update)

To properly launch this project, use the provided `run.py` script in the root directory of the repo.
```python run.py ./EmscriptenGLFW/```

It starts a local server with its root in the directory of the repository, such that it can also view the contents of `GPU-With-C-Sharp-Angular-WASM/data/` which are obtained by the built example at runtime using wget.

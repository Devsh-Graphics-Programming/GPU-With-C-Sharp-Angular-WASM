# C++ to WebAssembly using Emscripten

To solve one problem at a time instead of dealing with all of them at once, in this variant of the app we only look into how to compile the previously native app to WASM and get it running in the browser.

We are going to utilize the fact that Emscripten comes with its own GLFW and GLEW headers in the SDK.

## Findings

### Most of the C or C++ function definitions belonging to libraries in Emscripten's SDK are spoofed into JavaScript function calls

For example, there's no such thing as OpenGL ES 3.0 on the web, there's only a C header which uses Emscripten's JavaScript-Embedded-in-C macro to call WebGL 2 functions which are only usable from JavaScript.

Rest of the libraries, such as the standard C library, the Standard C++ Library, GLFW, SDL, etc. are bizarre hybrids where certain OS-specific API function calls are replaced with implementations that use JavaScript.

### Emscripten uses Clang but not llvm-project's `libcxx` and `libcxxabi`

Basically Emscripten compiles C++ into the LLVM-IR using Clang, but swaps out any headers declaring the API of the standard C and standard C++ libraries for its own, this is because a large swath of functionality such as `std::cout`, `std::mutex` or `std::thread` is implemented using some calls JavaScript as outlined above.

**This also means that Emscripten's Standard C++ Library is always a bit behind the curve, and as such it will warn you whenever you try to use a C++17 feature, and I wouldn't count on being able to use C++20 features unless they're trivial and not dependent on platform specific code to implement.**

The LLVM-IR is codegenned into WASM using Clang, but that functionality was merged from WASM-SDK's, WASI's and Emscripten's forks and patches.

_In practice one should not concern themselves with this detail, unless one is trying to make Emscripten use custom standard library headers from an LLVM fork like we did for the Reflection TS in our later projects._

### You cannot use a "main loop"

If your JS or WASM function does not return within a given timeout, the web-page's responsiveness freezes and you will eventually get a prompt from the browser whether to kill or wait for the script.

Any rendering or similar continuous operation needs to be implemented via callbacks instead of loops.

### There's no access to files, because they all get served from a server

Although Emscripten emulates a filesystem for the purposes of C's `fopen` and C++'s input file streams, so that code will compile, it emulates them in a virtual filesystem which can either be in memory (non persistent, like a RAM disk) or Indexed.DB (basically the browser's cache/persistent storage which can be wiped at any point and should not be relied on).

_This means that there's no way to get input data generated before the application start to your application, except for:_
1. _XHR Request/Fetch or a GET Request (files being served by the server)_
2. _Embedding the bytes of the resource by Emscripten into the WASM binary or "JS glue code"_

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

### Optimization Levels of O2 an O3 break debugging

Emscripten has a "closure compiler" which minifies the "glue JavaScript" and deletes any unused functions, this makes it impossible to debug the "glue JavaScript" without a source map.

Futhermore WASM function calls get aggressively inlined and DWARF symbols become lost or useless.

### Emscripten's WASM Target is 32bit!

The initial WASM MVP spec implemented in all major browsers specifies memory addresses/offsets to be 32bit.

_This means that `sizeof(void*)==4` when compiling C code with Emscripten!_

Aside from keeping the binary size a little smaller, it also make JS/WASM interop a little bit faster as JavaScript has no native 64bit integer type, however that only matters for pointer arithmetic which is rather done in WASM.

Also there's a `memory64` proposal which might make it into WASM 2.0 which would allow 64bit memory addresses and would make `sizeof(void*)==8`, for now Emscripten has a `-sMEMORY64=` option which you can set to the following:
- `0` off, 32bit memory
- `1` enabled and will emit 64bit WASM as per the proposal (this breaks most dependencies shipped with Emscripten's SDK)
- `2` will "spoof" 64bit pointers by keeping them 64bit on the WASM side (so `sizeof(void*)==8`) and 32bit on the JS side, then convert between them

We tested option 0 and 2, both worked.

## Modifications Required

### Game-Loop via callbacks

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

### File data reading

Also because of the aformentioned issue with access to files, we decided to go with the scalable option, which meant not embedding our images into the binary. As an excercise we used both `emscripten_async_wget_data` and `emscripten_wget_data` to load shaders and images from the server (which you need to run locally whenever you launch any WASM project).

### Compilation flags

Although Emscripten supports most of the Clang compiler flags, a few Emscripten specific compiler-flags needed to be added:
- `-sWASM=1` emit webassembly, otherwise everything (including your code) compiles JavaScript as it does with `asm.js`
- `-sWASM_BIGINT` enable 64bit integers to be used as native types in your C/C++
- `-sASYNCIFY` needed to have access to `emscripten_async_wget_data`
- `-sUSE_WEBGL2=1` enable the OpenGL ES to WebGL translation header and JS library
- `-sFULL_ES3=1` enable OpenGL ES 3.0 functions in the above (it actually comes with a non-trivial amount of JS wrapping code to emulate some functions which only exist in ES)
- `-sMIN_WEBGL_VERSION=2` disables any fallbacks in-case browser only implements WebGL 1.0 which would only present you with OpenGL ES 2.0 on the WASM side, useful if you only intend to write code for ES 3.0 and above, as it cuts down on the size of linked "JS glue code"
- `-sUSE_GLFW=3` to get access to GLFW header and JS library

## Building

As long as you have the Emscripten Target VS2022 component installed, the solution should load and work out-of-the-box.

The solution only has one project, build it by pressing `Ctrl + B` or straight up `F5` debugging it.

You can change the targets from Debug to RelWithDebInfo or Release.

## Debugging

**TL;DR All source-level CPU debugging features you're used to on Native, just work. For a discussion of GPU debugging using Renderdoc see the section in towards the bottom of the top level repository root README.md**

_However, all debugging in Release builds will be essentially non-fuctnional._

To debug this app, you can use either the Chrome Developer Tools Debugger once you run the example in Crhome or you can use the VS2022 debugger, which launches the example in Chrome and connects to the Chrome Developer Tools Debugger remotely.

Other than debugging C++, you can now also debug the generated JS glue code (a few thousand lines of it) that loads the wasm app and implements some of the functions that the Emscripten SDK declares in its C and C++ headers.

Disassembling the code yields reading from the generated `EmscriptenGLFW.wasm`


## Launching outside of the Debugger (TODO: update)

To properly launch this project, use the provided `run.py` script in the root directory of the repo.
```python run.py ./EmscriptenGLFW/```

It starts a local server with its root in the directory of the repository, such that it can also view the contents of `GPU-With-C-Sharp-Angular-WASM/data/` which are obtained by the built example at runtime using wget.

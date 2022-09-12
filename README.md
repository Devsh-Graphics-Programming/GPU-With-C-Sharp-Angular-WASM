# GPU-With-C-Sharp-Angular-WASM
A proof of concept of a WebAssembly app.

<br>

![Emscripten in chrome](./Docs/img3.jpg)

Is it possible to mix languages such as C# and C++ to make a Web app? This repo contains our findings of the pros and cons of WebAssembly, the process of compiling, debugging and maintaining code. 

We looked into how apps written purely in C#, C++ and mixed languages, compiled with mono-wasm, emscripten or blazor.

Each variant renders an interactable pathtraced scene using the same shader code.
The source for `./data/shader.frag` is [stochastic pathtracer v1 on shadertoy](https://www.shadertoy.com/view/4ddcRn) 


#### Controls:
 - Use `1, 2, 3, 4, 5` on alphanumerical keyboard to switch background.
 - The fifth background is a cubemap, that can be toggled between two variants by repeatedly pressing the `5` key.
 - Click and drag with a mouse on screen to change the position of a camera.

The apps were created in this order:

- Native
- EmscriptenGLFW
- EmscriptenHTML5
- EmscriptenHTML5SeparateLibs
- NativeCSharp
- BlazorEmscripten
  
And thus the notes are a followup of the previous chapters

# Installation
<br>

Clone this repository recursively

```
git clone https://github.com/Devsh-Graphics-Programming/GPU-With-C-Sharp-Angular-WASM.git --recurse-submodules 
```


<br>


### Emscripten

*EmscriptenGLFW, EmscriptenHTML5, EmscriptenHTML5SeparateLibs*

Instead of the standard C++ and C# compiler, the apps in this repository use Emscripten.

To install, clone the repository somewhere on your machine:

```
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk
```

Run the following emsdk commands to get the latest tools from GitHub and set them as active:
```
# Download and install the latest SDK tools.
./emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes .emscripten file)
./emsdk activate latest --permanent

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

Instruction on how to install Emscripten can be found on the [official site](https://emscripten.org/docs/getting_started/downloads.html).

Then, add an extra environment variable **EMSCRIPTEN**  with value `path-to-emsdk/upstream/emscripten`. We use i

Once thats done and all the environment variables are assigned *(with --permament option)* you can proceed.

Alas, Blazor Wasm will clone its own repo of `git@github.com:emscripten-core/emscripten.git` to `C:\Program Files\dotnet\packs\Microsoft.NET.Runtime.Emscripten.2.0.23.Sdk.win-x64\6.0.4\tools\emscripten\` or similiar version

<br>

----------------------------

### SWIG

*BlazorEmscripten only*

Hybrid apps require a language bridge. 

We use SWIG to generate platform invokes and native class equvalents in C#, and C-API functions for C++ public methods and fields.

Go to [SWIG download page](https://www.swig.org/download.html) and get the latest **swigwin** version (swigwin contains swig.exe and allows us to skip building on windows).

Extract the zip archive.

Add the extracted dir to PATH. 

<br>

----------------------------

### GLFW3

*Native only*

GLFW3 is needed to run the native C++ app and will automatically clone if you pass ` --recurse-submodules ` while cloning this repository.

<br>

----------------------------

### Visual Studio Extensions

*BlazorEmscripten only*

Install the following Visual Studio 2022 extensions
- .NET WebAssembly build tools
- Emscripten Build Target

<br>

----------------------------

### Chrome WASM C/C++ Devtools Extension

*BlazorEmscripten, EmscriptenGLFW, EmscriptenHTML5, EmscriptenHTML5SeparateLibs*

An extension for chrome to be able to debug C++ code with dwarf formatting. DWARF format contains source mapping to files on a local machine that compiled the app (absolute filepaths). It allows placing breakpoints and watches in source code as well as inspecting unmangled local variables. 

https://chrome.google.com/webstore/detail/cc%20%20-devtools-support-dwa/pdcpmagijalfljmkmjngeonclgbbannb

<br>

----------------------------

### Chrome DWARF experimental option

In order for the aforementioned extension to work an extra option needs to be enabled in the browser

- Open Chrome,
- Open Inspect Element (Ctrl + Shift + i) 
- Open Options (F1) ![Options](./Docs/img1.jpg)
- Go to Experiments 
- Enable WebAssembly Debugging: Enable DWARF Support ![DWARF support](./Docs/img2.jpg)

<br>


<br>

# License

See License.md


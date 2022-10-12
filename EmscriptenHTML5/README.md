# C++ in WASM without GLFW

In this iteration of the application, we get rid of GLFW and replace it with calls to Emscripten's C-bindings for HTML5 APIs. This means that the same code can no longer be compiled for Native and why the previous example served as a reference comparison between Native and WASM.

## Motivation

We noticed that SDL and GLFW are implemented on top of the Emscripten HTML5 header anyway and need to be maintained by Emscripten's team.

To simulate the development of a C++ WASM "Web First" application we thought that one would use HTML5 directly to target the web "natively".

Furthermore SDL and GLFW can only expose a subset of the HTML5 APIs, some of which might be useful for an XR application, such as cameras, GPS positioning, etc.

Also the SDL and GLFW wrappers add a lot of "JS glue code" to the final build output. 

## Findings

### One needs to deal with making the WebGL context current before rendering to a canvas

Overall, this is really similiar to using JS to initalize a WebGL/WebGL2 context, with the addition of making it work for static GL function calls.

### There's potential "Device Loss" to contend with

OpenGL ES will never lose your device, in WebGL your tab might get killed, overall its a situation similar to Vulkan device loss.

## Modifications Made

### Callbacks
- No longer uses glfw events in favor of html ones 
- Additional event that happens when mouse cursor is moved, and we keep track of it ourselves, as there is no static function to obtain mouse position inside render loop

### Window class
- Obtains html object with id `canvas`
- Sets the canvas context as webgl's current
- Assigns html callbacks to static methods/functions
- Resizes html cavas to desired dimensions

### Build Options

The `-sUSE_GLFW` compiler flag was removed, no special flag needed to get access to HTML5 headers.

## Building, Debugging and Launching

_No changes compared to the previous example._


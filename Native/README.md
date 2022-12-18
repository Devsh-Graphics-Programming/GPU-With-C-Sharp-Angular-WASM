# Reference C++ Native Application

To start the presentation, we prepared the most simple, OpenGL ES 3.0 native window app to make sure everything is working as intended outside of the browser. 

We constrained ourselves to only using dependencies that we knew Emscripten' SDK also provides such as GLFW and OpenGL ES.

Once we had a fully functional, traditional C++ app, we will see how much effort and how little code changes are required to get it working at comparable speeds in the browser.

## Building

We added GLFW and GLEW (OpenGL ES headers) as submodules to avoid using package managers such as `vcpkg` in our original CMake build system, which would later cause inconsistencies in how to configure and generate using CMake.

After finding out that VS2022 Emscripten Target projects support debugging from VS2022, we abandoned CMake as it would only be used for the Native C++ project. However keeping our dependencies as submodules was still useful for reducing the overall "invasiveness" of the setup procedure for the repository.

If you want to make sure the submodules are up to date and initialized after cloning, use the following commands from repository root:
```
cd Native
git submodule init
git submodule update
```

## Debugging

Debugging a native app is pretty straightforward using Visual Studio. Since this is the most basic variant of the app that doesnt even run in the web browser, we can consider this a reference for subsequent apps, and will look for a way to achieve the same options of debugging as we have available here:
- Inspecing locals/memory with original variable names
- Disassembly
- Displaying the stack with full variable names
- Placing Breakpoints in original sources
- Watches
- Debugging shaders/pipelines using RenderDoc
- etc.
 

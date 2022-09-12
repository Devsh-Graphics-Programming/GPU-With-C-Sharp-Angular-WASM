# GPU With C-Sharp Angular WASM
### Mixed Blazor C# .net core and C++ to WASM


Finally moving onto C#, in this variant of the app we are looking into creating a .NET core app that compiles with Emscripten to run in a browser. The obvious choice is to create a Blazor Webassembly Client app. (The server app only serves to do something like restricting access or providing routing to files)

Visual Studio allows us to create a Blazor WebAssembly Client project. Selecting this will set up a one-page example app and configure it to compile with Emscripten. As it is now, the default configuration of Blazor WebAssembly Client will download and use a separate `emcc.exe` compiler and `wasm-ld.exe` linker.

As for using purely C# written app, it's quite simple.
In the example, we used .NET Core libraries from NuGet Package Manager. It turns out that it is possible to use existing .dll libraries compiled in .NET in Blazor WebAssembly.


In this app, we take the Renderer library from EmscriptenHTML5SeparateLibs project, and call methods belonging to the Renderer class from C# code.

Generally speaking, instantiating a C++ class in C# comes with its own set of problems.
- C# uses GC, and will dispose of our class object when it sees fit, which is not neccessairly in line with it's used, and can be disposed before finish using it.
- Class fields and methods names are are mangled. For example, `void Renderer::change_cubemap` becomes `__ZN8Renderer14change_cubemapEv`.



We bypass these by using a C++ to C# language bride called SWIG.
It first generates C functions from C++ class methods, because C due to not supporting classes or overloads does not mangle names at all *(a class method in general is compiled to static function, where the first parameter is a pointer to the object. SWIG in theory creates duplicate code)*. Then, SWIG generates C# code:
- C# p/invokes as static methods to the generated C functions. 
Performance wise, C#'s platform invoke has a 30-40 CPU cycles overhead, which is neglible. 
- C# class that resembles the C++ class - has the same `public` methods and fields (using `get;set;`), and each method calls the p/invoke and returns its value. You can call 
`Renderer obj = new Renderer(1280, 720)` in both languages and both of these will execute the same C++ code.
The class also stores HandleRef object reference, such that it prevents garbage collection to prematurely take effect.



The LibRenderer project is the C++ library, its compiled with Emscripten into a relocatable wasm module with embedded DWARF.
BlazorEmscripten.Client is the single page WebAssembly .Net Core App.  
BlazorEmscripten.Server and BlazorEmscripten.Shared that comes with the project template were removed, because we dont use them here.

In order to link the Blazor App with LibRenderer, we use NativeFileReference introduced in dotnet 6.0.
Since both apps compile to Binaryen, we can statically link the two into a single wasm module. 
Dynamic linking using NativeFileReference is added to dotnet 8.0 milestone. [(Github issue)](https://github.com/dotnet/runtime/issues/75257)




`BlazorEmscripten.Client/wwwroot/scripts/renderloop.js` continously invokes a C# method, which calls the C++ render method the same way as `emscripten_set_main_loop` does it. 



<br>

# Installation
<br>

## Emscripten

Automatically downloaded by Visual Studio

## SWIG, VS & Chrome extensions
As explained in [readme.md](../README.md) in directory above

## Dotnet runtime

It might be required for you download an updated version of dotnet runtime if the app doesnt launch.

<br>

Upon opening BlazorEmscripten.sln a request will appear in the solution explorer in case you dont have the needed VS components installed.
<br>

## Building

First, build LibRenderer project. It has a pre-build event to run swig.

Then build Client. It has a pre build event to remove exception handling code from the generated swig files that fails compilation due to missing `UnmanagedCallersOnly` attributes. It also has a post build event to copy `../../data` to `wwwroot/data` to avoid having to dealt with routing static files. 

<br>

-----

# Launching

Run using visual studio start debugger button (or with F5) or with `dotnet run` command


<br><br><br><br><br>

# Debugging

Debugging C# and JS is possible from within Visual Studio. C# does not differ much from debugging a native app. Debugging C++ from VS is impossible.

Debuggin C++ and JS is possible from within Chrome debugger like in previous app variants, however it is impossible to debug C# this way. 





Notes:
-------

In order to achieve better performance, the Blazor Wasm project uses AOT compilation

We dont know if mono supports dlopen. It can be that any requests for dll import return null 
[#if WASM_SUPPORTS_DLOPEN](https://github.com/mono/mono/blob/5d2e3bc3b3c8184d35b2f7801e88d96470d367c4/sdks/wasm/src/driver.c#L195)

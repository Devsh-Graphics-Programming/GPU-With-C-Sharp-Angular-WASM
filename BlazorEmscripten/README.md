# GPU With C-Sharp Angular WASM
## C++ and C# Compiled to WASM with Emscripten

<br>

#Summary

In this app, we combine a C++ library, and call methods belonging to a class in C# code.
The LibRenderer project is the C++ library, its compiled with Emscripten into a relocatable wasm module.
BlazorEmscripten.Client is the single page WebAssembly .Net Core App.  
BlazorEmscripten.Server is http server that provides routing to `../../data` and its contents.

`BlazorEmscripten.Client/wwwroot/scripts/renderloop.js` continously invokes a C# method, which calls the C++ render method.
Performance wise, PINVOKE has 

<br>

# Installation
<br>

## Emscripten

Set up Emscripten as explained in readme in directory above

<br>

Open BlazorEmscripten.sln
A request will appear in the solution explorer in case you dont have the neccessary installed VS components.
<br>

## Building
First, build LibRenderer project
Then in solution explorer navigate to `BlazorEmscripten.Client > SWIG > LibRendererPINVOKE.cs` and as a teporary work around comment out classes SWIGExceptionHelper, SWIGPendingException, SWIGStringHelper (lines 14-188)
From the context menu go to `build > build all`
<br>

-----

# Launching

The example needs to download a shader at runtime, thus there needs to be a running http server at the same port. Please run the BlazorEmscripten.Server first to provide static file hosting needed for the page to work.
Run using visual studio start debugger button (or with F5)


<br><br><br><br><br>

Notes:
-------

In order to achieve better performance, the Blazor Wasm project uses AOT compilation

We dont know if mono supports dlopen. It can be that any requests for dll import return null 
[#if WASM_SUPPORTS_DLOPEN](https://github.com/mono/mono/blob/5d2e3bc3b3c8184d35b2f7801e88d96470d367c4/sdks/wasm/src/driver.c#L195)

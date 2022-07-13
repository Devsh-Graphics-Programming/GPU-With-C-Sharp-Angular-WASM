# GPU With C-Sharp Angular WASM
## C++ Native Sample

<br>

# Controls:
 - Use `1, 2, 3, 4, 5` on alphanumerical keyboard to switch background.
 - The fifth background is a cubemap, that can be toggled between two variants by repeatedly pressing the `5` key.
 - Click with a mouse on screen to change the position of a camera.

<br>

# Installation

Start by installing [VCPKG](https://vcpkg.io/en/getting-started.html) and setting it up.
I recommend adding `vcpkg` to PATH as it will make using it simpler.
Once thats done, we will use this tool to install glfw3 and glew with the following commands in powershell:

>```vcpkg install glfw3``` 

>```vcpkg install glew``` 

<br>

Next, we need to configure CMake.
Run the following command in command line to obtain a filepath neccessary for CMAKE_TOOLCHAIN_FILE configuration:

>```vcpkg integrate install```

<br>
the result should look something like this: 

```
Applied user-wide integration for this vcpkg root.

All MSBuild C++ projects can now #include any installed libraries.
Linking will be handled automatically.
Installing new libraries will make them instantly available.

CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

<br>

Open CMAKE GUI at `GPU-With-C-Sharp-Angular-WASM` directory and before configuring for the first time add a filepath entry with name `CMAKE_TOOLCHAIN_FILE`. The value is the filepath obtained via `vcpkg integrate install` command.

Once thats done, configure with CMake. 

To build this example, select `BUILD_NATIVE` option.

Last step is to click the Generate in CMake GUI to obtain project files.

----

With this, you are done with installation and you can now compile and run the C++ Native Sample 

<br><br><br><br><br>

Notes:
-------
- The example uses a fragment shader locaded in a relative path at `../../data/shader.frag`
- Similarly, it uses cube maps located in a relative path `../../data/env/`

Therefore, carelessly moving the exectuable file will break the app.

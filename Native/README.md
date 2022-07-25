# GPU With C-Sharp Angular WASM
## C++ Native Sample

<br>

# Installation

The native example uses glfw3, and to make the installation easier, it is added as a submodule.
Make sure the submodule is present and up to date by adding `--recurse-submodules` to `git clone` or by using these commands:
```
git submodule init
git submodule update
```


<br>

Then, configure this directory with CMake. This and only this target can use Visual Studio 2022 as generator.



<br><br><br><br><br>

Notes:
-------
- The example uses a fragment shader locaded in a relative path at `../../data/shader.frag`
- Similarly, it uses cube maps located in a relative path `../../data/env/`

Therefore, carelessly moving the exectuable file will break the app.

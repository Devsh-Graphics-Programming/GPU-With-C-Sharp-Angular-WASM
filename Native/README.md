# GPU With C-Sharp Angular WASM
## C++ Native Sample

<br>

# Controls:
 - Use `1, 2, 3, 4, 5` on alphanumerical keyboard to switch background.
 - The fifth background is a cubemap, that can be toggled between two variants by repeatedly pressing the `5` key.
 - Click and drag with a mouse on screen to change the position of a camera.

<br>

# Installation

The native example uses glfw3, and to make the installation easier, it is added as a submodule.
Make sure the submodule is present and up to date using the following commands:
```
git submodule init
git submodule update
```


Then, configure this directory with CMake, preferably with Visual Studio 2022 as target.



<br><br><br><br><br>

Notes:
-------
- The example uses a fragment shader locaded in a relative path at `../../data/shader.frag`
- Similarly, it uses cube maps located in a relative path `../../data/env/`

Therefore, carelessly moving the exectuable file will break the app.

# GPU With C-Sharp Angular WASM
## C++ Native Sample

<br>

# Controls:
 - Use `1, 2, 3, 4, 5` on alphanumerical keyboard to switch background.
 - The fifth background is a cubemap, that can be toggled between two variants by repeatedly pressing the `5` key.
 - Click with a mouse on screen to change the position of a camera.

<br>

# Installation
run 
```
git submodule update
```


Then, configure this directory with CMake.



<br><br><br><br><br>

Notes:
-------
- The example uses a fragment shader locaded in a relative path at `../../data/shader.frag`
- Similarly, it uses cube maps located in a relative path `../../data/env/`

Therefore, carelessly moving the exectuable file will break the app.

# GPU With C-Sharp Angular WASM
## C++ Native

To start the presentation, we prepared the most simple, native window app to make sure everything is working as intended outside of the browser. 

Once we have a fully functional, traditional C++ app, we will see how far we can push it with minimal code changes to get it working at comparable speeds in the browser.


<br>

# Installation

## Dependencies

GLFW

The native example uses glfw3, and to make the installation easier, it is added as a submodule.
Make sure the submodule is present and up to date by adding `--recurse-submodules` to `git clone` or by using these commands:
```
git submodule init
git submodule update
```

GLEW

[Download link](http://glew.sourceforge.net/)

<br>

CMake 

Configure this directory with CMake. This and only this target in this repository can use Visual Studio 2022 as generator.



<br><br><br><br><br>




Notes:
-------
- The example uses a fragment shader locaded in a relative path at `../../data/shader.frag`
- Similarly, it uses cube maps located in a relative path `../../data/env/`

Therefore, carelessly moving the exectuable file will break the app.

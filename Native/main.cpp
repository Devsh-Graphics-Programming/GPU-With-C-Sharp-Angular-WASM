/*
GLFW3 + GLEW example of a sample scene, written in C++

Read about controlls and building in Native/README.md


Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.

*/

#include "window.h"


int main() {
	Window w = Window(1280, 720);

	while (w.keepAlive())
		w.update();

}
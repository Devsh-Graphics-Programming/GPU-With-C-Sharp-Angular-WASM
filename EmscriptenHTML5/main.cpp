/*
GLFW3 + GLEW example of a sample scene, written in C++

Read about controlls and building in Native/README.md


Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.

*/

#include "window.h"



Window* w;
void main_loop()
{
	w->update();
}

int main() {
	w = new Window(1280, 720);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (w->keepAlive())
		w->update();
#endif

}
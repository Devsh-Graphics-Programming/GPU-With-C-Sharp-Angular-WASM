/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#ifndef __WINDOW_H_INCLUDED__
#define __WINDOW_H_INCLUDED__


#include "renderer.h"
#include <emscripten/html5_webgl.h>

//window class contains html5 functions to initialize a canvas, assign callbacks for key presses and mouse actions, and rendering 
class Window {
private:
	Renderer* m_renderer;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;

public:
	bool m_mouse_down;

	Window(int width, int height);

	void update();

	void onMouseDrag(long xpos, long ypos);

	~Window();

	Renderer* getRenderer();
};

#endif // !__WINDOW_H_INCLUDED__
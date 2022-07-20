/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#ifndef __WINDOW_H_INCLUDED__
#define __WINDOW_H_INCLUDED__

#include "renderer.h"


//window class contains glfw3 functions to initialize a window, handle key presses, mouse button clicks and finalizes renderering of the frame into the created glfwwindow
class Window {
private:
	GLFWwindow* m_window;
	Renderer* m_renderer;

public:
	bool m_mouse_down;

	Window(int window_width, int window_height);

	bool keepAlive();

	void update();

	void onMouseDrag();

	~Window();

	Renderer* getRenderer();
};

#endif // !__WINDOW_H_INCLUDED__
/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "renderer.h"
#include "callbacks.h"
#include "window.h"
#include <iostream>
#include <emscripten/key_codes.h>

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
	if (eventType == EMSCRIPTEN_EVENT_KEYPRESS) {
		int env = -1;
		switch (e->keyCode)
		{
		case DOM_VK_ESCAPE:
			emscripten_cancel_main_loop();
			return 1;
		case DOM_VK_5: //change the background to load a cubemap texture. clicking it again toggles texture
			env = 5;
			break;
		case DOM_VK_4: //change background to black and white gradient
			env = 4;
			break;
		case DOM_VK_3: //change background to procedural desert
			env = 3;
			break;
		case DOM_VK_2: //change background to procedural 4 pointlight scene
			env = 2;
			break;
		case DOM_VK_1: //change background to none
			env = 1;
			break;
		default:
			break;
		}

		if (env != -1) {
			Window* window = (Window*)userData;
			Renderer* renderer = window->getRenderer();
			renderer->Set_iEnv(env);
			if (env == 5)
				renderer->change_cubemap();
			std::cout << "Changed background to " << env << std::endl;
			return 1;

		}
	}
	return 0;
}


EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent* e, void* userData) {
	Window* window = (Window*)userData;

	if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
	{
		if (e->button == 0) {
			std::cout << "Mouse down" << std::endl;
			window->m_mouse_down = true;
			return 1;
		}
	}
	else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
		if (e->button == 0) {
			std::cout << "Mouse up" << std::endl;
			window->m_mouse_down = false;
			return 1;
		}
	}
	else if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
	{
		window->onMouseDrag(e->clientX, e->clientY);
		return 1;
	}
	return 0;

}
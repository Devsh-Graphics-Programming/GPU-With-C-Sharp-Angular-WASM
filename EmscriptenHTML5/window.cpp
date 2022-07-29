/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "window.h"
#include <stdlib.h>
#include "callbacks.h"

Window::Window(int width, int height) {

	EmscriptenWebGLContextAttributes attr;
	emscripten_webgl_init_context_attributes(&attr);
	attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
	attr.enableExtensionsByDefault = 1;
	attr.premultipliedAlpha = 0;
	attr.majorVersion = 4;
	attr.minorVersion = 1;
	context = emscripten_webgl_create_context("#canvas", &attr);
	emscripten_webgl_make_context_current(context);

	m_mouse_down = false;

	//assign callbacks for mouse movement and clicks
	emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, mouse_callback);
	emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, mouse_callback);
	emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, mouse_callback);
	
	emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, key_callback);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		exit(-1);
	}

	emscripten_set_canvas_element_size("#canvas", width, height);

	//create renderer 
	m_renderer = new Renderer(width, height);
}

void Window::update() {
	m_renderer->Set_iTime(emscripten_get_now() / 1000.0f);

	// main loop, render, blit to back buffer and swap front&back buffers
	m_renderer->render();
}

Window::~Window() {
	delete m_renderer;
}


Renderer* Window::getRenderer() {
	return m_renderer;
}


void Window::onMouseDrag(long xpos, long ypos) {
	if (m_mouse_down) 
	{
		m_renderer->Set_iMouse((GLfloat)xpos, (GLfloat)ypos);
	}

}
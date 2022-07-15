/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "window.h"
#include <stdlib.h>
#include "callbacks.h"

Window::Window(int window_width, int window_height) {
	if (!glfwInit())
		exit(-1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(window_width, window_height, "Fullscreen Triangle /w frag shader", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(m_window);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	glfwSetKeyCallback(m_window, key_callback);
	
	//bugs out with __EMSCRIPTEN__
	//glfwSwapInterval(1);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		glfwTerminate();
		exit(-1);
	}

	//create renderer 
	m_renderer = new Renderer(window_width, window_height);
	glfwSetWindowUserPointer(m_window, m_renderer);
}

bool Window::keepAlive() {
	return !glfwWindowShouldClose(m_window);
}

void Window::update() {
	glfwPollEvents();
	m_renderer->render();
	m_renderer->Set_iTime(glfwGetTime());
	glfwSwapBuffers(m_window);
}

Window::~Window() {
	delete m_renderer;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

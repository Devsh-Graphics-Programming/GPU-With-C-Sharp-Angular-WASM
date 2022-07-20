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

	m_mouse_down = false;

	//GLFW_STICKY_MOUSE_BUTTONS is not implemented in emscripten
	//glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE); // set mouse button clicks to be 'sticky' - make sure the release callback is always invoked

	glfwSetMouseButtonCallback(m_window, mouse_button_callback); // called only on button down and button up	glfwSetKeyCallback(m_window, key_callback);
	glfwSetKeyCallback(m_window, key_callback);

	//not needed with __EMSCRIPTEN__
	//glfwSwapInterval(1);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		glfwTerminate();
		exit(-1);
	}

	//create renderer 
	m_renderer = new Renderer(window_width, window_height);
	glfwSetWindowUserPointer(m_window, this);
}

bool Window::keepAlive() {
	return !glfwWindowShouldClose(m_window);
}

void Window::update() {
	//handle input
	glfwPollEvents();
	if (m_mouse_down)
		onMouseDrag();

	m_renderer->Set_iTime(glfwGetTime());

	// main loop, render, blit to back buffer and swap front&back buffers
	m_renderer->render();
	glfwSwapBuffers(m_window);
}

Window::~Window() {
	delete m_renderer;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


Renderer* Window::getRenderer() {
	return m_renderer;
}


void Window::onMouseDrag() {
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos); // get cursor position on screen
	m_renderer->Set_iMouse(xpos, ypos);

}
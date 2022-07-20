/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "renderer.h"
#include "callbacks.h"
#include "window.h"
#include <iostream>

//callback for handling mouse clicks
//mouse input changes camera position
void mouse_button_callback(GLFWwindow* m_window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {

		Window* window = (Window*)glfwGetWindowUserPointer(m_window);
		if (action == GLFW_PRESS)
		{
			std::cout << "Mouse down" << std::endl;
			window->m_mouse_down = true;
		}
		else if (action == GLFW_RELEASE) {
			std::cout << "Mouse up" << std::endl;
			window->m_mouse_down = false;
		}
	}
}

//handle keyboard button presses
void key_callback(GLFWwindow* m_window, int key, int scancode, int action, int mods)
{
	int env = -1;
	if (action == GLFW_PRESS)
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(m_window, GLFW_TRUE);
			break;
		case GLFW_KEY_5: //change the background to load a cubemap texture. clicking it again toggles texture
			env = 5;
			break;
		case GLFW_KEY_4: //change background to black and white gradient
			env = 4;
			break;
		case GLFW_KEY_3: //change background to procedural desert
			env = 3;
			break;
		case GLFW_KEY_2: //change background to procedural 4 pointlight scene
			env = 2;
			break;
		case GLFW_KEY_1: //change background to none
			env = 1;
			break;
		default:
			break;
		}
	if (env != -1) {
		Window* window = (Window*)glfwGetWindowUserPointer(m_window);
		Renderer* renderer = window->getRenderer();
		renderer->Set_iEnv(env);
		if (env == 5)
			renderer->change_cubemap();
		std::cout << "Changed background to " << env << std::endl;

	}
}

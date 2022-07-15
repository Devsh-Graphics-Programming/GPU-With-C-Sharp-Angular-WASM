/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#ifndef __RENDERER_H_INCLUDED__
#define __RENDERER_H_INCLUDED__


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class Renderer {

private:

	static constexpr int FBOCOUNT = 2,
		CUBEMAP_COUNT = 2;
	int window_width, window_height, frame_num, current_cubemap;
	GLint iMouse_location, iChannel0_location, iChannel1_location, iTime_location, iResolution_location, iEnv_location;
	GLFWwindow* window;
	GLuint program,
		FBOs[FBOCOUNT],
		render_textures[FBOCOUNT],
		cubemaps[CUBEMAP_COUNT];
	GLuint VAO;
	
	void clear_render();
	void init_cubemaps();

public:
	Renderer(int w, int h);
	void change_cubemap();
	void render();
	void Set_iMouse(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void Set_iEnv(GLint env);
	void Set_iTime(GLfloat t);
};

#endif // !__RENDERER_H_INCLUDED__

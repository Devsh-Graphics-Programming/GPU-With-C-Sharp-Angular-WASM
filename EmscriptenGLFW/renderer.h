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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif

class Renderer {

private:

	static constexpr int FBOCOUNT = 2,
		CUBEMAP_COUNT = 2;
	int	m_window_width = -1,
		m_window_height = -1,
		m_frame_counter = -1,
		m_cubemap_index = -1;

	GLint	m_iMouse_location = -1,
		m_iChannel0_location = -1,
		m_iChannel1_location = -1,
		m_iTime_location = -1,
		m_iResolution_location = -1,
		m_iEnv_location = -1,
		m_iFramesRcp_location = -1;

	GLFWwindow* m_window;

	GLuint m_program,
		m_VAO,
		m_FBOs[FBOCOUNT],
		m_render_textures[FBOCOUNT],
		m_cubemaps[CUBEMAP_COUNT];
	bool m_setup_complete;

	void clear_render();
	void init_cubemaps();
	void load_shader(const char* shaderPath, GLenum shaderType);

public:
	int m_shaders_loaded = 0;
	GLuint getProgram();
	void init();

	Renderer(int w, int h);
	void change_cubemap();
	void render();
	void Set_iMouse(GLfloat x, GLfloat y);
	void Set_iEnv(GLint env);
	void Set_iTime(GLfloat t);
};

#endif // !__RENDERER_H_INCLUDED__

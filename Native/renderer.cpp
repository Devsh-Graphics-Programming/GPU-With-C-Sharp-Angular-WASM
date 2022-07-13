/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "renderer.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"



/// <summary>
/// helper function to load 6 images and arrange them into a cubemap
/// </summary>
static constexpr int CUBE_SIDES_NUM = 6;
GLuint load_cubemap_from_file(std::string faces[], std::string& env_path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int window_width, window_height, nrChannels;
	for (unsigned int i = 0; i < CUBE_SIDES_NUM; i++)
	{
		unsigned char* data = stbi_load((env_path + faces[i]).c_str(), &window_width, &window_height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
			std::cout << "Cubemap tex successfuly loaded: " << faces[i] << std::endl;

		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}

//create cubemaps
void Renderer::init_cubemaps() {
	std::cout << "Loading cubemaps" << std::endl;

	current_cubemap = 0;
	std::string storage_folder_dir = "../../data/env/";
	std::string files[2][6]{
		{"back.jpg",   "front.jpg",   "top.jpg",   "bottom.jpg",   "right.jpg",   "left.jpg"},
		{"back1.jpg",  "front1.jpg",  "top1.jpg",  "bottom1.jpg",  "right1.jpg",  "left1.jpg"}
	};
	for (size_t i = 0; i < CUBEMAP_COUNT; i++)
	{
		cubemaps[i] = load_cubemap_from_file(files[i], storage_folder_dir);
	}
}

//toggle between cubemap backgrounds
void Renderer::change_cubemap() {
	current_cubemap = (current_cubemap + 1) % CUBEMAP_COUNT;
}

//check if shaders compiled successfuly
void Renderer::checkShaderStatus(GLuint obj)
{
	GLint status = GL_FALSE;
	if (glIsShader(obj)) glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (glIsProgram(obj)) glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_TRUE) return;
	GLchar log[1 << 16] = { 0 };
	if (glIsShader(obj)) glGetShaderInfoLog(obj, sizeof(log), NULL, log);
	if (glIsProgram(obj)) glGetProgramInfoLog(obj, sizeof(log), NULL, log);
	std::cerr << log << std::endl;
	exit(-1);
}

//create a shader from string
void Renderer::load_shader(const char* shaderCode, GLenum shaderType, GLuint program)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	checkShaderStatus(shader);
	glAttachShader(program, shader);
	glDeleteShader(shader);
}

//clear accumulated image
void Renderer::clear_render() {
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glClear(GL_COLOR_BUFFER_BIT);

	}
}

//main render loop 
void Renderer::render() {

	int i = frame_num & 1;
	int ii = i ^ 1;
	glClearColor(0, 0, 0, 1);
	//first, draw to a framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);

	//use previous frame render result as accumulation buffer
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_textures[ii]);
	glActiveTexture(GL_TEXTURE0);

	//bind current cubemap
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[current_cubemap]);
	glUniform1i(iChannel1_location, 2);

	//bind shaders and render a full screen triangle
	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//draw contents of a framebuffer to glfw window buffer (buffer 0)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBOs[0]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//wait for end of frame
	glFinish();
	frame_num++;
}


void Renderer::Set_iMouse(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glProgramUniform4f(program, iMouse_location, x, y, z, w);
	clear_render();
}


void Renderer::Set_iEnv(GLint env) {
	glProgramUniform1i(program, iEnv_location, env);
	clear_render();
}


void Renderer::Set_iTime(GLfloat t) {
	glProgramUniform1f(program, iTime_location, t);
}

Renderer::Renderer(int w, int h) {
	window_width = w;
	window_height = h;
	program = glCreateProgram();

	//enabkle necessary features
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_2D);

	//create FBOs
	glGenFramebuffers(FBOCOUNT, FBOs);
	glGenTextures(FBOCOUNT, render_textures);
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glBindTexture(GL_TEXTURE_2D, render_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_textures[i], 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//load a simple oneliner vertex shader
	load_shader(vertex_shader, GL_VERTEX_SHADER, program);

	//load text file containing fragment shader code
	//source https://www.shadertoy.com/view/4ddcRn
	std::ifstream file("../../data/shader.frag");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string frag = buffer.str();

	load_shader(frag.c_str(), GL_FRAGMENT_SHADER, program);
	glLinkProgram(program);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	float data[] = //full screen triangle, FS tri > FS quad for complex fragment shaders
	{
		-1.0f,-1.0f,
		3.0f, -1.0f,
		-1.0f,3.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	GLuint index_buffer = 0;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	unsigned int indexes[] =
	{
		0,1,2
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	iResolution_location = glGetUniformLocation(program, "iResolution");
	iTime_location = glGetUniformLocation(program, "iTime");
	iMouse_location = glGetUniformLocation(program, "iMouse");
	iChannel0_location = glGetUniformLocation(program, "iChannel0");
	iChannel1_location = glGetUniformLocation(program, "iChannel1");
	iEnv_location = glGetUniformLocation(program, "iEnv");

	init_cubemaps();

	glProgramUniform3f(program, iResolution_location, window_width, window_height, 0);
	glProgramUniform4f(program, iMouse_location, 0, 0, 0, 0);
	glUniform1i(iChannel0_location, 1);

	glViewport(0, 0, window_width, window_height);
	frame_num = 0;
	std::cout << "Renderer created\n";
}

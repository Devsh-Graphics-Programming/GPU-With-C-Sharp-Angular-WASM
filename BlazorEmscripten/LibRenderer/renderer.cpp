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

const std::string data_url = "https://localhost:7152/data/";

//clear accumulated image
void Renderer::clear_render() {
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOs[i]);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	m_frame_counter = 0u;
}


unsigned int Renderer::getProgram() { return m_program; }


void Renderer::Set_iMouse(float x, float y)
{
	glUniform2f(m_iMouse_location, x, y);
	clear_render();
}


void Renderer::Set_iEnv(int env)
{
	glUniform1i(m_iEnv_location, env);
	clear_render();
}


void Renderer::Set_iTime(float t)
{
	glUniform1f(m_iTime_location, t);
}

// function to load 6 images and arrange them into a cube map
// uses synchronous wget
static constexpr int CUBE_SIDES_NUM = 6;
GLuint load_cubemap_from_wget(std::string faces[], std::string& env_path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int window_width, window_height, nrChannels;
	for (unsigned int i = 0; i < CUBE_SIDES_NUM; i++)
	{
		std::string url = env_path + faces[i];
		void* outBuffer; 
		int outSize, outError;
		emscripten_wget_data(url.c_str(), &outBuffer, &outSize, &outError);

		if (outError) //if error code is non-zero, then wget failed
		{
			std::cerr << "Cubemap tex failed to load from url=" << url << "\t\t error code : " << outError << std::endl;
			continue;
		}

		unsigned char* data = stbi_load_from_memory((unsigned char*)(outBuffer), outSize, &window_width, &window_height, &nrChannels, 0);

		free(outBuffer);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
			std::cout << "Cubemap tex successfuly loaded: " << faces[i] << "\t\tSize: " << outSize << std::endl;

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

	m_cubemap_index = 0;
	std::string storage_folder_dir = data_url + "env/";
	std::string files[2][6]{
		{"back.jpg",   "front.jpg",   "top.jpg",   "bottom.jpg",   "right.jpg",   "left.jpg"},
		{"back1.jpg",  "front1.jpg",  "top1.jpg",  "bottom1.jpg",  "right1.jpg",  "left1.jpg"}
	};
	for (size_t i = 0; i < CUBEMAP_COUNT; i++)
	{
		m_cubemaps[i] = load_cubemap_from_wget(files[i], storage_folder_dir);
	}
}


//toggle between cubemap backgrounds
void Renderer::change_cubemap() {
	m_cubemap_index = (m_cubemap_index + 1) % CUBEMAP_COUNT;
}


//check if shaders compiled successfuly
void checkGLStatus(GLuint obj)
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


void on_shader_data_obtained(void* args, void* data, int size) {
	Renderer* renderer = (Renderer*)args;
	GLenum shaderType = renderer->m_shaders_loaded++ == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	std::cout << "Loaded shader " << (shaderType==GL_VERTEX_SHADER? "GL_VERTEX_SHADER":"GL_FRAGMENT_SHADER") << std::endl;

	const char* charactersBegin = (const char*)(data);
	std::string shaderCode(charactersBegin, size);
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &charactersBegin, NULL);
	glCompileShader(shader);
	checkGLStatus(shader);
	glAttachShader(renderer->getProgram(), shader);
	glDeleteShader(shader);

	renderer->init();
}


//asynchronously begin downloading shader code
void Renderer::load_shader(const char* shaderPath, GLenum shaderType)
{

	std::string url = data_url + std::string(shaderPath);
	std::cout << "Loading shader " << url;
	emscripten_async_wget_data(url.c_str(), this, on_shader_data_obtained, nullptr);
}


Renderer::Renderer(int w, int h) {
	std::cout << "Creating renderer";
	EmscriptenWebGLContextAttributes attr;
	emscripten_webgl_init_context_attributes(&attr);
	attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
	attr.enableExtensionsByDefault = 1;
	attr.premultipliedAlpha = 0;
	attr.majorVersion = 3;
	attr.minorVersion = 0;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context("#canvas", &attr);
	emscripten_webgl_make_context_current(context);

	m_window_width = w;
	m_window_height = h;
	m_program = glCreateProgram();

	//create FBOs
	glGenFramebuffers(FBOCOUNT, m_FBOs);
	glGenTextures(FBOCOUNT, m_render_textures);
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOs[i]);
		glBindTexture(GL_TEXTURE_2D, m_render_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_window_width, m_window_height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_render_textures[i], 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	std::cout << "Framebuffers initialized";

	m_shaders_loaded = 0;
	//load a simple oneliner vertex shader
	load_shader("shader.vert", GL_VERTEX_SHADER);

	//load text file containing fragment shader code
	//source https://www.shadertoy.com/view/4ddcRn
	load_shader("shader.frag", GL_FRAGMENT_SHADER);


	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

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
	std::cout << "VAO initialized";


}

void Renderer::init()
{
	if (m_shaders_loaded < 2) return;

	glLinkProgram(m_program);
	checkGLStatus(m_program);

	m_iResolution_location = glGetUniformLocation(m_program, "iResolution");
	m_iTime_location = glGetUniformLocation(m_program, "iTime");
	m_iMouse_location = glGetUniformLocation(m_program, "iMouse");
	m_iChannel0_location = glGetUniformLocation(m_program, "iChannel0");
	m_iChannel1_location = glGetUniformLocation(m_program, "iChannel1");
	m_iEnv_location = glGetUniformLocation(m_program, "iEnv");
	m_iFramesRcp_location = glGetUniformLocation(m_program, "iFramesRcp");

	init_cubemaps();

	glUseProgram(m_program);
	glUniform2i(m_iResolution_location, m_window_width, m_window_height);
	glUniform2f(m_iMouse_location, 0, 0);
	glUniform1i(m_iChannel0_location, 0);
	glUniform1i(m_iChannel1_location, 1);
	glUseProgram(0);

	glViewport(0, 0, m_window_width, m_window_height);
	m_frame_counter = 0;

	m_setup_complete = true;
	std::cout << "Renderer created\n";
}	


//main render loop 
void Renderer::render() {
	if (!m_setup_complete)return;

	int i = m_frame_counter & 1;
	int ii = i ^ 1;

	glClearColor(0, 0, 0, 1);
	//first, draw to a framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOs[i]);

	//use previous frame render result as accumulation buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_render_textures[ii]);

	//bind current cubemap
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemaps[m_cubemap_index]);

	//bind shaders and render a full screen triangle
	glUseProgram(m_program);
	glBindVertexArray(m_VAO);
	glUniform1f(m_iFramesRcp_location, 1.f / float(++m_frame_counter));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//draw contents of a framebuffer to glfw window buffer (buffer 0)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOs[0]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_window_width, m_window_height, 0, 0, m_window_width, m_window_height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//wait for end of frame
	glFinish();
}

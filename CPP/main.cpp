#define DEBUG

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

constexpr int WIDTH = 1024;
constexpr int HEIGHT = 720;
constexpr int FBOCOUNT = 2;

const char* vertex_shader = R"(
#version 430 core
layout( location = 0 ) in vec2 fragCoord;
void main()
{
    gl_Position = vec4( fragCoord, 0.0, 1.0 );

}
)";

#ifdef DEBUG
//frag shader for testing, not used in the example
const char* fragment_shader = R"(
#version 430 core

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform sampler2D iChannel0;          // input channel. XX = 2D/Cube
uniform float     timeInSeconds;                 // (year, month, day, time in seconds)

out vec4 FragColor;
void main()
{
    FragColor = vec4(sin(iTime + 3.1415f), 0.5f, 0.2f, 1.0f);
} 

)";
void CheckStatus(GLuint obj)
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
#endif

void LoadShader(const char* shaderCode, GLenum shaderType, GLuint program)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
#ifdef DEBUG
	CheckStatus(shader);
#endif
	glAttachShader(program, shader);
	glDeleteShader(shader);
}




GLuint program;
GLint iMouse_location, iChannel0_location, iTime_location, iResolution_location, iEnv_location;

GLuint fbos[FBOCOUNT];
GLuint tex[FBOCOUNT];

void clear_render() {
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
		glClear(GL_COLOR_BUFFER_BIT);

	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		std::cout << "Cursor Position at (" << xpos << " : " << ypos << "\n";
		glProgramUniform4f(program, iMouse_location, xpos, ypos, 0, 0);
		clear_render();
	}
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int env = -1;
	if (action == GLFW_PRESS)
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_4:
			env = 4;
			break;
		case GLFW_KEY_3:
			env = 3;
			break;
		case GLFW_KEY_2:
			env = 2;
			break;
		case GLFW_KEY_1:
			env = 1;
			break;
		default:
			break;
		}
	if (env != -1) {
		glProgramUniform1i(program, iEnv_location, env);
		clear_render();
	}
}


int main() {
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fullscreen Triangle /w frag shader", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}

	program = glCreateProgram();

	glEnable(GL_TEXTURE_2D);



	glGenFramebuffers(FBOCOUNT, fbos);
	glGenTextures(FBOCOUNT, tex);
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[i], 0);
		glClear(GL_COLOR_BUFFER_BIT);

	}


	LoadShader(vertex_shader, GL_VERTEX_SHADER, program);

	//load text file containing fragment shader code from https://www.shadertoy.com/view/4ddcRn
	std::ifstream file("../fragmentshader.txt");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string frag = buffer.str();

	LoadShader(frag.c_str(), GL_FRAGMENT_SHADER, program);
	//LoadShader(fragment_shader, GL_FRAGMENT_SHADER, program);
	glLinkProgram(program);


	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	float data[] = //full screen triangle, i read on stack overflow that FS tri> FS quad for complex fragment shaders
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

	std::cout << "Starting\n";

	iResolution_location = glGetUniformLocation(program, "iResolution");
	iTime_location = glGetUniformLocation(program, "iTime");
	iMouse_location = glGetUniformLocation(program, "iMouse");
	iChannel0_location = glGetUniformLocation(program, "iChannel0");
	iEnv_location = glGetUniformLocation(program, "iEnv");

	glProgramUniform3f(program, iResolution_location, WIDTH, HEIGHT, 0);
	glProgramUniform1f(program, iTime_location, (float)glfwGetTime());
	glProgramUniform4f(program, iMouse_location, 0, 0, 0, 0);
	glUniform1i(iChannel0_location, 1);

	glViewport(0, 0, WIDTH, HEIGHT);
	int frame = 0;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		int i = frame & 1;
		int ii = i ^ 1;
		glClearColor(0, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex[ii]);
		glActiveTexture(GL_TEXTURE0);
		glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (i) {
			//draw to screen
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glfwSwapBuffers(window);
		}
		
		glFinish();
		glfwPollEvents();
			
		frame++;

		glProgramUniform1f(program, iTime_location, glfwGetTime());
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

class Renderer {

private:

	static constexpr int FBOCOUNT = 2;
	int width = 1024;
	int height = 720;

	GLint iMouse_location, iChannel0_location, iTime_location, iResolution_location, iEnv_location;
	GLFWwindow* window;
	GLuint program, fbos[FBOCOUNT], tex[FBOCOUNT];
	GLuint VAO;
	int frame;
	const char* vertex_shader = R"(
#version 430 core
layout( location = 0 ) in vec2 fragCoord;
void main()
{
    gl_Position = vec4( fragCoord, 0.0, 1.0 );

}
)";


	void checkShaderStatus(GLuint obj)
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

	void load_shader(const char* shaderCode, GLenum shaderType, GLuint program)
	{
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);
		checkShaderStatus(shader);
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}

	void clear_render() {
		for (size_t i = 0; i < FBOCOUNT; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
			glClear(GL_COLOR_BUFFER_BIT);

		}
	}

public:

	void render() {

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

		//draw to screen
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glFinish();
		frame++;
	}

	void Set_iMouse(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		glProgramUniform4f(program, iMouse_location, x, y, z, w);
		clear_render();
	}
	void Set_iEnv(GLint env) {
		glProgramUniform1i(program, iEnv_location, env);
		clear_render();
	}
	void Set_iTime(GLfloat t) {
		glProgramUniform1f(program, iTime_location, t);
	}
	Renderer(int w, int h) {
		width = w;
		height = h;
		program = glCreateProgram();
		glEnable(GL_TEXTURE_2D);
		glGenFramebuffers(FBOCOUNT, fbos);
		glGenTextures(FBOCOUNT, tex);
		for (size_t i = 0; i < FBOCOUNT; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[i], 0);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		load_shader(vertex_shader, GL_VERTEX_SHADER, program);
		//load text file containing fragment shader code from https://www.shadertoy.com/view/4ddcRn
		std::ifstream file("../data/shader.frag");
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string frag = buffer.str();

		load_shader(frag.c_str(), GL_FRAGMENT_SHADER, program);
		//LoadShader(fragment_shader, GL_FRAGMENT_SHADER, program);
		glLinkProgram(program);

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
		
		iResolution_location = glGetUniformLocation(program, "iResolution");
		iTime_location = glGetUniformLocation(program, "iTime");
		iMouse_location = glGetUniformLocation(program, "iMouse");
		iChannel0_location = glGetUniformLocation(program, "iChannel0");
		iEnv_location = glGetUniformLocation(program, "iEnv");

		glProgramUniform3f(program, iResolution_location, width, height, 0);
		glProgramUniform1f(program, iTime_location, (float)glfwGetTime());
		glProgramUniform4f(program, iMouse_location, 0, 0, 0, 0);
		glUniform1i(iChannel0_location, 1);

		glViewport(0, 0, width, height);
		frame = 0;
		std::cout << "Renderer created\n";
	}
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		std::cout << "Cursor Position at (" << xpos << " : " << ypos << "\n";
		Renderer* renderer = (Renderer*)glfwGetWindowUserPointer(window);
		renderer->Set_iMouse(xpos, ypos, 0, 0);
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
		Renderer* renderer = (Renderer*)glfwGetWindowUserPointer(window);
		renderer->Set_iEnv(env);
	}
}

class Window {
	GLFWwindow* m_window;
	Renderer* m_renderer;

public:
	Window(int width, int height) {
		if (!glfwInit())
			exit(-1);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, "Fullscreen Triangle /w frag shader", NULL, NULL);
		if (!m_window)
		{
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(m_window);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
		glfwSetKeyCallback(m_window, key_callback);
		glfwSwapInterval(1);

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			glfwTerminate();
			exit(-1);
		}

		//create renderer 
		m_renderer = new Renderer(width, height);
		glfwSetWindowUserPointer(m_window, m_renderer);
	}

	bool keepAlive() {
		return !glfwWindowShouldClose(m_window);
	}

	void update() {
		glfwPollEvents();
		m_renderer->render();
		m_renderer->Set_iTime(glfwGetTime());
		glfwSwapBuffers(m_window);
	}

	~Window() {
		delete m_renderer;
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
};

int main() {
	Window w = Window(1280, 720);


	//BEGONE INFINITE LOOP! EMSCRIPT SHAN'T TOLERATE THIS HERESY 
	while (w.keepAlive())
	{
		w.update();
	}

}
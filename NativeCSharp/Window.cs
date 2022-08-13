using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Arqan;

using static Arqan.GLFW;

namespace NativeCSharp
{
	public class Window
	{
		IntPtr window;
		readonly Renderer renderer;

		bool mouseDown;

		public Window(int width, int height)
		{
			if(glfwInit() == 0)
			{
				throw new Exception("GLFW could not be initialized");
			}
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_SRGB_CAPABLE, 1);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

			byte[] title = Encoding.ASCII.GetBytes("Fullscreen Triangle /w frag shader");
			window = glfwCreateWindow(width, height, title, IntPtr.Zero, IntPtr.Zero);
			if(window == IntPtr.Zero)
			{
				glfwTerminate();
				throw new Exception("GLFW window not created");
			}
			glfwMakeContextCurrent(window);
			mouseDown = false;
			glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE); // set mouse button clicks to be 'sticky' - make sure the release callback is always invoked

			GLFWmousebuttonfun mousebuttonCallback = (win, button, action, mods) =>
			{
				if(button == GLFW_MOUSE_BUTTON_LEFT)
				{
					mouseDown = action == GLFW_PRESS;
					Console.WriteLine(mouseDown ? "Mouse down" : "Mouse up");
				}
			};
			GLFWkeyfun keyCallback = (win, key, scancode, action, mods) =>
			{
				int env = -1;
				if(action == GLFW_PRESS)
					switch(key)
					{
						case GLFW_KEY_ESCAPE:
							glfwSetWindowShouldClose(window, GLFW_TRUE);
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
				if(env != -1)
				{
					if(env == 5)
						renderer.NextCubemap();
					renderer.SetEnv(env);
					Console.WriteLine("Changed background to " + env);

				}
			};

			glfwSetMouseButtonCallback(window, mousebuttonCallback); // called only on button down and button up
			glfwSetKeyCallback(window, keyCallback);
			glfwSwapInterval(1);

			renderer = new Renderer(width, height);

		}

		public bool KeepAlive => glfwWindowShouldClose(window) != GLFW_TRUE;

		public void Update()
		{
			glfwPollEvents();
			if(mouseDown)
				MouseDrag();
			renderer.SetTime((float) glfwGetTime());
			renderer.Render();
			glfwSwapBuffers(window);
		}

		~Window()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		private void MouseDrag()
		{
			double x = 0, y = 0;
			glfwGetCursorPos(window, ref x, ref y);
			renderer.SetMousePos((float) x, (float) y);
		}
	}
}

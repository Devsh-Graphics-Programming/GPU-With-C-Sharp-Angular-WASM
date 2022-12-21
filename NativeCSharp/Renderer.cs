using System.Drawing;
using System.Drawing.Imaging;
using Arqan;
using static Arqan.GL;

namespace NativeCSharp
{
	public class Renderer
	{
		const int FBOCOUNT = 2;
		const int CUBEMAPCOUNT = 2;

		uint iMouseLocation, iEnvLocation, iTimeLocation, iResolutionLocation, iChannel0Location, iChannel1Location, iFrameRcpLocation;
		uint[] fbos, cubemaps, renderTextures;
		int frameCounter, selectedCubemap;
		uint program, vao;

		int height, width;

		public void ClearRender()
		{
			for(int i = 0; i < FBOCOUNT; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);
			}
			frameCounter = 0;
		}

		public void SetMousePos(float x, float y)
		{
			glUniform2f(iMouseLocation, x, y);
			ClearRender();
		}

		public void SetEnv(int env)
		{
			glUniform1i(iEnvLocation, env);
			ClearRender();
		}

		public void SetTime(float t)
		{
			glUniform1f(iTimeLocation, t);
		}

		public void NextCubemap() 
		{ 
			selectedCubemap = (selectedCubemap + 1) % CUBEMAPCOUNT;
		}


		private uint LoadCubemap(string[] faces, in string path)
		{
			uint[] texture = new uint[1];
			glGenTextures(1, texture);
			uint textureId = texture[0];
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

			for(uint i = 0; i < 6; i++)
			{
				//Create a new bitmap
				Bitmap bmp = new Bitmap(path + faces[i]);

				// Lock the bitmap's bits. 
				BitmapData bmpData = bmp.LockBits(
					   new Rectangle(0, 0, bmp.Width, bmp.Height),
					  ImageLockMode.ReadOnly,
					  bmp.PixelFormat);

				// Get the address of the first line
				IntPtr ptr = bmpData.Scan0;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, bmp.Width, bmp.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			return textureId;
		}

		private void InitCubeMaps()
		{
			Console.WriteLine("Loading cubemaps");
			selectedCubemap = 0;
			string storageDir = "../../../../data/env/";
			string[][] faces = new string[][] {
				new [] {"back.jpg",   "front.jpg",   "top.jpg",   "bottom.jpg",   "right.jpg",   "left.jpg"},
				new [] {"back1.jpg",  "front1.jpg",  "top1.jpg",  "bottom1.jpg",  "right1.jpg",  "left1.jpg"}
			};
			cubemaps = new uint[CUBEMAPCOUNT];
			for(int i = 0; i < CUBEMAPCOUNT; i++)
			{
				cubemaps[i] = LoadCubemap(faces[i], storageDir);
			}
		}

		private void CheckGlStatus(uint obj)
		{
			const int size = 1 << 16;

			if(glIsShader(obj))
			{
				int status = (int) GL_FALSE;
				glGetShaderiv(obj, GL_COMPILE_STATUS, ref status);
				if(status == GL_TRUE) return;
				byte[] logbuffer = new byte[size];
				int len = 0;
				glGetShaderInfoLog(obj, size, ref len, logbuffer);
				var msg = System.Text.Encoding.Default.GetString(logbuffer, 0, len);
				throw new Exception(msg);
			}
			{
				int[] status = new int[1];
				if(glIsProgram(obj)) glGetProgramiv(obj, GL_LINK_STATUS, status);
				if(status[0] == GL_TRUE) return;
				byte[] logbuffer = new byte[size];
				int len = 0;
				glGetProgramInfoLog(obj, size, ref len, logbuffer);
				var msg = System.Text.Encoding.Default.GetString(logbuffer, 0, len);
				throw new Exception(msg);
			}

		}


		void LoadShader(string shaderPath, uint shaderType)
		{
			var shaderCode = new string[] { File.ReadAllText("../../../../data/" + shaderPath) };
			uint shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, shaderCode, null);
			glCompileShader(shader);
			CheckGlStatus(shader);
			glAttachShader(program, shader);
			glDeleteShader(shader);
		}

		public Renderer(int width, int height)
		{
			this.width = width;
			this.height = height;
			glFlush();
			program = glCreateProgram();

			glEnable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_2D);

			fbos = new uint[FBOCOUNT];
			renderTextures = new uint[FBOCOUNT];
			glGenFramebuffers(FBOCOUNT, fbos);
			glGenTextures(FBOCOUNT, renderTextures);
			for(int i = 0; i < FBOCOUNT; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
				glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, IntPtr.Zero);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextures[i], 0);
				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);
			}

			//load a simple oneliner vertex shader
			LoadShader("shader.vert", GL_VERTEX_SHADER);

			//load text file containing fragment shader code
			//source https://www.shadertoy.com/view/4ddcRn
			LoadShader("shader.frag", GL_FRAGMENT_SHADER);

			glLinkProgram(program);
			CheckGlStatus(program);

			var vaos = new uint[1] { 0u };
			glGenVertexArrays(1, vaos);
			vao = vaos[0];
			glBindVertexArray(vao);
			uint[] buffers = new uint[1] { 0u };
			glGenBuffers(1, buffers);
			uint buffer = buffers[0];
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			float[] data = new float[]
			{
				-1.0f,-1.0f,
				3.0f, -1.0f,
				-1.0f,3.0f
			};
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, data, GL_STATIC_DRAW);
			glGenBuffers(1, buffers);
			var indexBuffer = buffers[0];
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			uint[] indexes = new uint[]
			{
				0,1,2
			};
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3, indexes, GL_STATIC_DRAW);
			glEnableVertexArrayAttrib(vao, 0);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, IntPtr.Zero);
			glBindVertexArray(0);

			iResolutionLocation = glGetUniformLocation(program, "iResolution");
			iTimeLocation = glGetUniformLocation(program, "iTime");
			iMouseLocation = glGetUniformLocation(program, "iMouse");
			iChannel0Location = glGetUniformLocation(program, "iChannel0");
			iChannel1Location = glGetUniformLocation(program, "iChannel1");
			iEnvLocation = glGetUniformLocation(program, "iEnv");
			iFrameRcpLocation = glGetUniformLocation(program, "iFramesRcp");

			InitCubeMaps();

			glUseProgram(program);
			glUniform2i(iResolutionLocation, width, height);
			glUniform2f(iMouseLocation, 0, 0);
			glUniform1i(iChannel0Location, 0);
			glUniform1i(iChannel1Location, 1);
			glUseProgram(0);

			glViewport(0, 0, width, height);
			frameCounter = 0;
			Console.WriteLine("Renderer setup complete");

		}

		public void Render()
		{
			int current = frameCounter & 1;
			int other = current ^ 1;

			glClearColor(1, 1, 0, 1);
			glBindFramebuffer(GL_FRAMEBUFFER, fbos[current]);

			//set accumulation texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderTextures[other]);
			//set cubemap
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[selectedCubemap]);

			//bind shaders and render a full screen triangl to current fbo
			glUseProgram(program);
			glBindVertexArray(vao);
			glUniform1f(iFrameRcpLocation,  1f / (++frameCounter));
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, IntPtr.Zero);
			glBindVertexArray(0);

			//draw contents of a framebuffer to glw window buffer(buffer 0)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glFinish();
		}
	}
}

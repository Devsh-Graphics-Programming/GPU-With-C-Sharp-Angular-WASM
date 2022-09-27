using System.Net.NetworkInformation;


using Microsoft.AspNetCore.Components.Web;
using Microsoft.AspNetCore.Components.WebAssembly.Hosting;
using Microsoft.JSInterop;

using System.Net.Http;
using System.Net.Http.Json;
using Microsoft.AspNetCore.Components.Forms;
using Microsoft.AspNetCore.Components.Routing;
using Microsoft.AspNetCore.Components.Web;
using Microsoft.AspNetCore.Components.Web.Virtualization;
using Microsoft.AspNetCore.Components.WebAssembly.Http;
using Microsoft.JSInterop;
using Microsoft.AspNetCore.Components;

namespace Example
{
	public class Program
	{




		public static async Task Main(string[] args)
		{
			var builder = WebAssemblyHostBuilder.CreateDefault(args);
			builder.RootComponents.Add<App>("#app");
			await builder.Build().RunAsync();
			Console.WriteLine("Stop");
		}
	}

	public class App : IComponent
	{
		Renderer renderer;
		DateTime start;
		bool mouseHeldDown;


		[Inject] public IJSRuntime JS { get; set; }



		//needs to be a public parameterless constructor 
		public App()
		{
			start = DateTime.Now;
			Console.WriteLine("Ctor");

		}
		internal async Task Start(RenderHandle renderHandle)
		{
			Console.WriteLine("JS is " + JS);
			await JS.InvokeVoidAsync("alert", "hello");



			renderer = new Renderer(1280, 720);
			while(true)
			{
				await renderHandle.Dispatcher.InvokeAsync(() => RenderInBlazor(renderHandle));
			}
		}


		//user input
		protected void MouseMove(MouseEventArgs e)
		{
			//Console.WriteLine($"X = {e.ClientX} Y = {e.ClientY}");
			if(mouseHeldDown)
				renderer.Set_iMouse((float) e.ClientX, (float) e.ClientY);
		}


		protected void MouseClickDown(MouseEventArgs e)
		{
			mouseHeldDown = true;
		}


		protected void MouseClickUp(MouseEventArgs e)
		{
			mouseHeldDown = false;
		}


		private void KeyPress(KeyboardEventArgs args)
		{
			if(int.TryParse(args.Key, out int result) && result >= 0 && result < 6)
			{
				if(result == 5)
					renderer.change_cubemap();

				renderer.Set_iEnv(result);
			}
		}



		private void RenderInBlazor(RenderHandle renderHandle)
		{
			//set time uniform in frag shader
			var timespan = DateTime.Now - start;
			renderer.Set_iTime((float) timespan.TotalSeconds);

			//render to active gl context - canvas
			renderer.render();
		}

		public void Attach(RenderHandle renderHandle)
		{
			Console.WriteLine("Attached");
			renderHandle.Dispatcher.InvokeAsync(()=>Start(renderHandle));
		}
		public Task SetParametersAsync(ParameterView parameters) {
			Console.WriteLine(parameters);
			return Task.CompletedTask;
		}
	}
}
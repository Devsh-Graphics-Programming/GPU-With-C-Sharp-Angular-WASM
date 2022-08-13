using Microsoft.AspNetCore.ResponseCompression;
using Microsoft.Extensions.FileProviders;
using System.IO;

namespace BlazorEmscripten
{
	public class Program
	{
		public static void Main(string[] args)
		{
			var builder = WebApplication.CreateBuilder(args);

			// Add services to the container.

			builder.Services.AddControllersWithViews();
			builder.Services.AddRazorPages();

			var app = builder.Build();

			// Configure the HTTP request pipeline.
			if(app.Environment.IsDevelopment())
			{
				app.UseWebAssemblyDebugging();
			}
			else
			{
				app.UseExceptionHandler("/Error");
				// The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
				app.UseHsts();
			}

			app.UseHttpsRedirection();

			app.UseBlazorFrameworkFiles();
			var path = Path.Combine(Directory.GetCurrentDirectory(), "../../data");
			app.UseStaticFiles(new StaticFileOptions
			{
				FileProvider = new PhysicalFileProvider(path),
				ServeUnknownFileTypes = true,
				RequestPath = "/Data"
			});
			app.UseStaticFiles();

			app.UseDirectoryBrowser(new DirectoryBrowserOptions
			{
				FileProvider = new PhysicalFileProvider(path),
				RequestPath = "/Data"
			});
			app.UseFileServer(enableDirectoryBrowsing: true);


			app.UseRouting();


			app.MapRazorPages();
			app.MapControllers();
			app.MapFallbackToFile("index.html");

			app.Run();
		}
	}
}
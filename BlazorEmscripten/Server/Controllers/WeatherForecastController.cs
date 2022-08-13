using Microsoft.AspNetCore.Mvc;

namespace BlazorEmscripten.Server.Controllers
{
	[Route("api/[controller]")]
	public class DataFetcher : Controller
	{
		[HttpGet("[action]")]
		public byte[] GetData(string action, string id)
		{
			Console.WriteLine(System.IO.Directory.GetCurrentDirectory());
			return System.IO.File.ReadAllBytes("../../../" + action);
		}
	}
}
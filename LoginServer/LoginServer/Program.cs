using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Owin.Hosting;

namespace LoginServer
{
	class Program
	{
		static void Main(string[] args)
		{
			string baseAddress = "http://*.19000/";

			using (WebApp.Start<Startup>(url: baseAddress))
			{
				var client = new HttpClient();

				var response = client.GetAsync(baseAddress + "api/values").Result;

				Console.WriteLine(response);
				Console.WriteLine(response.Content.ReadAsStringAsync().Result);

				Console.ReadLine();
			}
		}
	}
}

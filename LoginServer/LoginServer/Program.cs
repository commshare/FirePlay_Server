using Microsoft.Owin.Hosting;
using System;

namespace LoginServer
{
	internal class Program
	{
		private static void Main(string[] args)
		{
			const string baseAddress = "http://localhost:19000/";

			using (WebApp.Start<Startup>(url: baseAddress))
			{
				// TODO :: 로그인 서버 설정파일 읽어오기.
				Console.WriteLine("Login Server 실행 중 : " + baseAddress);
				Console.ReadLine();
			}
		}
	}
}

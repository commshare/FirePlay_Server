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
			string baseAddress = "http://localhost:19000/";

			using (WebApp.Start<Startup>(url: baseAddress))
			{
				Console.WriteLine("Login Server 실행 중 : " + baseAddress);
				// TODO :: 로그인 서버 설정파일 읽어오기.
				Console.ReadLine();
			}
		}
	}
}

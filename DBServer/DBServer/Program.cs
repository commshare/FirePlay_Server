using Microsoft.Owin.Hosting;
using System;

namespace DBServer
{
    class Program
    {
        static void Main(string[] args)
        {
            // TODO :: DB 서버 설정파일 읽어오기.
            const string baseAddress = "http://localhost:20000/";

            using (WebApp.Start<Startup>(url: baseAddress))
            {
                var result = DBServerMain.Init();

                if (result != ErrorCode.None)
                {
                    Console.WriteLine();
                    return;
                }

                Console.WriteLine("DB Server 실행 중 : " + baseAddress);
                Console.ReadLine();
            }
        }
    }
}

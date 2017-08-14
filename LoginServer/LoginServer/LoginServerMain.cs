using System;

namespace LoginServer
{
	public static class LoginServerMain
	{
		public static ErrorCode Init()
		{
			var error = InitDb();

			// 에러코드가 이상할 경우 출력해준다.
			if (error != ErrorCode.None) Console.WriteLine($"Starting failed in InitDb : {error}");

			return error;
		}

		private static ErrorCode InitDb()
		{
			const string redisList = "localhost:6379";
			var error = DB.RedisManager.Init(redisList);

			return error;
		}
	}
}

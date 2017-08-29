using System;

namespace DBServer
{
    public static class DBServerMain
    {
        public static ErrorCode Init()
        {
            var error = InitDB();

            // 에러가 이상할 경우 출력.
            if (error != ErrorCode.None) Console.WriteLine($"Starting failed in InitDB : {error}");

            return error;
        }

        private static ErrorCode InitDB()
        {
            const string redisList = "localhost:6379";
            var error = DB.RedisManager.Init(redisList);

            return error;
        }
    }
}

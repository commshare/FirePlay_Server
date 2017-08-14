using System;
using System.Linq;
using System.Threading.Tasks;
using CloudStructures;

namespace LoginServer.DB
{
	public static class RedisManager
	{
		private static RedisGroup _redisGroupBasic;

		public static ErrorCode Init(string address)
		{
			try
			{
				var basicRedisConnectionString = address.Split(',').ToList();
				var redisSettings = new RedisSettings[basicRedisConnectionString.Count()];

				if (!basicRedisConnectionString.Any()) return ErrorCode.RedisInvalidAddressString;

				for (var i = 0; i < basicRedisConnectionString.Count(); ++i)
				{
					redisSettings[i] = new RedisSettings(basicRedisConnectionString[i], db:0);	
				}

				_redisGroupBasic = new RedisGroup(groupName:"Basic", settings: redisSettings);

				// 서버마다 초기에 연결 한번 해놓는다. (막상 필요할 때 연결하려면 느리니까)
				for (var i = 0; i < basicRedisConnectionString.Count(); ++i)
				{
					var key = i.ToString() + "_test";
					var redis = new RedisString<int>(_redisGroupBasic, key);
					var result = redis.Set(11);

					if (result.Result == false)
					{
						return ErrorCode.RedisStartSetTestFailed;
					}
				}

				return ErrorCode.None;
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				return ErrorCode.RedisStartException;
			}
		}

		// float을 사용할 수 없다(Redis). 대신 double을 사용하자.
		public static async Task<bool> SetStringAsync<T>(string key, T dataObject)
		{
			try
			{
				var redis = new RedisString<T>(_redisGroupBasic, key);
				await redis.Set(dataObject);
				return true;
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				return false;
			}
		}

		public static async Task<bool> SetStringAsyncWhenNoExists<T>(string key, T dataObject)
		{
			try
			{
				var redis = new RedisString<T>(_redisGroupBasic, key);
				var result = await redis.Set(dataObject, null, StackExchange.Redis.When.NotExists);
				return result;
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				return false;
			}
		}

		public static async Task<Tuple<bool, T>> GetStringAsync<T>(string key)
		{
			try
			{
				var redis = new RedisString<T>(_redisGroupBasic, key);
				var value = await redis.Get();

				return value.Value == null ? Tuple.Create(false, default(T)) : Tuple.Create(true, value.Value);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				throw;
			}	
		}
	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;
using MongoDB.Driver;
using MongoDB.Driver.Core.Configuration;

namespace LoginServer.MongoDB
{
	public class MongoDBManager
	{
		private const string _connectionString = "mongodb://localhost:27017/?maxPoolSize=200";

		private const string _userDbName = "UserDB";

		private const string _collectionName = "UserLoginInfo";

		public static IMongoCollection<T> GetCollection<T>(string dbName, string collectionName)
		{
			var database = GetMongoDatabase(dbName);
			var collection = database.GetCollection<T>(collectionName);
			return collection;
		}

		public static IMongoDatabase GetMongoDatabase(string dbName)
		{
			MongoClient mongoClient = new MongoClient(_connectionString);
			return mongoClient.GetDatabase(dbName);
		}

		public static async Task<UserVaildation> GetUserVaildtion(DbUser reqUserInfo)
		{
			// 리턴할 구조체를 생성한다.
			var userValidate = new UserVaildation();
			userValidate.Id = reqUserInfo.Id;

			// MongoDB에서 해당하는 유저의 정보를 찾아본다.
			var collection = GetCollection<DbUser>(_userDbName, _collectionName);
			DbUser data;

			try
			{
				data = await collection.Find(x => x.Id == reqUserInfo.Id).FirstOrDefaultAsync();
			}
			catch (ArgumentNullException e)
			{
				// 유저 정보가 없다면, 새로 만들어준다.
				await collection.InsertOneAsync(reqUserInfo);

				// 새로 만들었다는 사실을 구조체에 적어놓고 반환한다..
				userValidate.Result = ErrorCode.ReqLoginNewId;
				return userValidate;
			}

			// 유저 정보가 있는데 INVALID한지 검사한다.
			if (data.Pw != reqUserInfo.Pw)
			{
				// INVALID하다면 오류를 적어놓는다.				
				userValidate.Result = ErrorCode.ReqLoginInvalidPw;
			}
			else
			{
				// 정상적인 경우라면 정상 값을 적어놓는다.
				userValidate.Result = ErrorCode.None;
			}

			// 해당 구조체를 리턴해준다.
			return userValidate;
		}
	}

	public struct DbUser
	{
		public string Id;
		public string Pw;
	}

	public struct UserVaildation
	{
		public string Id;
		public ErrorCode Result;
	}
}
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;
using MongoDB.Driver;
using MongoDB.Driver.Core.Configuration;

namespace LoginServer
{
	public class MongoDbManager
	{
		private const string ConnectionString = "mongodb://localhost:27017/?maxPoolSize=200";

		private const string UserDbName = "UserDB";

		private const string CollectionName = "UserLoginInfo";

		public static IMongoCollection<T> GetCollection<T>(string dbName, string collectionName)
		{
			var database = GetMongoDatabase(dbName);
			var collection = database.GetCollection<T>(collectionName);
			return collection;
		}

		public static IMongoDatabase GetMongoDatabase(string dbName)
		{
			MongoClient mongoClient = new MongoClient(ConnectionString);
			return mongoClient.GetDatabase(dbName);
		}

		public static async Task<UserVaildation> GetUserVaildtion(DbUser reqUserInfo)
		{
			// 리턴할 구조체를 생성한다.
			var userValidate = new UserVaildation();
			userValidate.Id = reqUserInfo.Id;

			// MongoDB에서 해당하는 유저의 정보를 찾아본다.
			var collection = GetCollection<DbUser>(UserDbName, CollectionName);
			DbUser data;

			try
			{
				data = await collection.Find(x => x.Id == reqUserInfo.Id).FirstOrDefaultAsync();
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				userValidate.Result = ErrorCode.MongoDbFindError;

				return userValidate;
			}

			if (data.Id == null)
			{
				// 유저 정보가 없다면, 에러를 적고 반환해준다.
				userValidate.Result = ErrorCode.ReqLoginInvalidId;
				return userValidate;
			}

			// 패스워드가 일치한다면 정상값을, 일치하지 않는다면 에러 값을 적어준다.
			userValidate.Result = (data.Pw != reqUserInfo.Pw) ? ErrorCode.ReqLoginInvalidPw : ErrorCode.None;

			// 해당 구조체를 리턴해준다.
			return userValidate;
		}

		public static async Task<UserVaildation> JoinUserValidation(DbUser joinUserInfo)
		{
			// 리턴할 구조체를 생성한다.
			var userValidate = new UserVaildation();
			userValidate.Id = joinUserInfo.Id;

			// MongoDB에서 아이디가 일치하는 유저가 있는지 찾아본다.
			var collection = GetCollection<DbUser>(UserDbName, CollectionName);
			DbUser data;

			try
			{
				data = await collection.Find(x => x.Id == joinUserInfo.Id).FirstOrDefaultAsync();
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				userValidate.Result = ErrorCode.MongoDbFindError;

				return userValidate;
			}

			// 유저 정보가 있다면, 이미 유저정보가 있다고 적어놓고 반환한다.
			if (data.Id != null)
			{
				String debugLabel = "Join Request " + joinUserInfo.Id + "/" + joinUserInfo.Pw + "Failed";
				Console.WriteLine(debugLabel);
				userValidate.Result = ErrorCode.ReqLoginIdAlreadyExist;
				return userValidate;		
			}

			// 유저 정보가 없다면, 새로 만들어준다.
			await collection.InsertOneAsync(joinUserInfo);

			// 성공적이라고 적어놓고 반환한다.
			userValidate.Result = ErrorCode.None;

			String successLabel = "Join Request " + joinUserInfo.Id + "/" + joinUserInfo.Pw + "Success";
			Console.WriteLine(successLabel);

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
using MongoDB.Driver;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DBServer.MongoDB
{
    public class DBUser
    {
        public Int64 UId;
        public string _id;
        public string _pw;
    }

    public struct UserValidation
    {
        public string Id;
        public ErrorCode Result;
    }

    class MongoDBManager
    {

        // TODO :: 로컬 호스트에서 바꾸어 주어야 함.
        private const string ConnectionString = "mongodb://localhost:27017/?maxPoolSize=200";

        private const string UserDBName = "UserDB";

        private const string LoginCollectionName = "UserLoginInfo";

        private const string ScoreCollectionName = "UserScoreInfo";

        public static IMongoDatabase GetMongoDatabase(string dbName)
        {
            MongoClient mongoClient = new MongoClient(ConnectionString);
            return mongoClient.GetDatabase(dbName);
        }

        public static IMongoCollection<T> GetCollection<T>(string dbName, string collectionName)
        {
            var database = GetMongoDatabase(dbName);
            var collection = database.GetCollection<T>(collectionName);
            return collection;
        }

        // Login시 유저 정보가 맞는지를 확인해주는 함수.
        public static async Task<UserValidation> GetUserValidation(string userId, string userPw)
        {
            var userValidation = new UserValidation();
            userValidation.Id = userId;

            // Find MongoDB
            var collection = GetCollection<DBUser>(UserDBName, LoginCollectionName);
            DBUser userData;

            try
            {
                userData = await collection.Find(x => x._id == userId).FirstOrDefaultAsync();
            }
            catch (Exception e)
            {
                // 에러가 난 경우 에러를 적고 반환.
                Console.WriteLine(e.Message);
                userValidation.Result = ErrorCode.MongoDBFindError;

                return userValidation;
            }

            if (string.IsNullOrEmpty(userData._id))
            {
                // 유저 정보가 없다면 에러 반환.
                userValidation.Result = ErrorCode.ValidationInvalidId;
            }
            else
            {
                // 패스워드가 일치한다면 정상 값을, 일치하지 않는다면 에러값을 적어준다.
                userValidation.Result = (userData._pw != userPw) ? ErrorCode.ValidationInvalidPw : ErrorCode.None;
            }

            return userValidation;
        }

        // MongoDB에서 아이디가 일치하는 유저가 있는지 확인하는 메소드.
        public static async Task<bool> GetIdAlreadyExist(string userId)
        {
            var collection = GetCollection<DBUser>(UserDBName, LoginCollectionName);
            DBUser data;

            try
            {
                data = await collection.Find(x => x._id == userId).FirstOrDefaultAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return false;
            }

            return data != null;
        }

        // MongoDB에 유저를 등록하는 메소드.
        public static async Task<UserValidation> GetJoinResult(string userId, string userPw)
        {
            var userValidation = new UserValidation();
            userValidation.Id = userId;

            var collection = GetCollection<DBUser>(UserDBName, LoginCollectionName);
            DBUser data;

            try
            {
                data = await collection.Find(x => x._id == userId).FirstOrDefaultAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                userValidation.Result = ErrorCode.MongoDBFindError;
                return userValidation;
            }

            // 유저 정보가 이미 있다면, 유저 정보가 있다고 적어놓고 반환한다.
            if (data != null)
            {
                userValidation.Result = ErrorCode.SignInIdAlreadExist;
                return userValidation;
            }

            // 유저 정보가 없다면, 새로 넣어준다.
            var newUser = new DBUser()
            {
                _id = userId,
                UId = DateTime.Now.Ticks,
                _pw = userPw
            };

            await collection.InsertOneAsync(newUser);

            userValidation.Result = ErrorCode.None;

            return userValidation;
        }
    }
}

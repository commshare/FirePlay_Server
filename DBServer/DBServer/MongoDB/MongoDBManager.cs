using MongoDB.Driver;
using System;
using System.Threading.Tasks;

namespace DBServer.MongoDB
{
    public class DBUser
    {
        public string _id;
        public string _pw;
        public Int64 UId;
        public int _wins = 0;
        public int _loses = 0;
    }
    
    class MongoDBManager
    {
        private const string ConnectionString = "mongodb://localhost:27017/?maxPoolSize=200";

        private const string UserDbName = "UserDB";

        public const string collectionName = "UserInfo";

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

        // 해당하는 정보의 유저가 가입되어 있는지를 확인해주는 메소드.
        public static async Task<ErrorCode> IsUserExist(string userName, string encryptedUserPw)
        {
            var collection = GetCollection<DBUser>(UserDbName, collectionName);
            DBUser data;

            try
            {
                data = await collection.Find(x => x._id == userName).FirstOrDefaultAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return ErrorCode.MongoDBFindError;
            }

            if (data == null)
            {
                return ErrorCode.None;
            }
            else
            {
                return ErrorCode.LoginUserInfoDontExist;
            }
        }

        // MongoDB에 지정한 ID, PW를 등록시켜주는 메소드.
        public static async Task<ErrorCode> AddUser(string userName, string encrytedUserPw)
        {
            // 일단 해당하는 정보의 유저가 이미 있는지를 확인.
            var checkValidation = await IsUserExist(userName, encrytedUserPw);
            // 이미 존재한다면 에러코드 반환.
            if (checkValidation == ErrorCode.None)
            {
                return ErrorCode.LoginUserInfoAlreadyExist;
            }

            // 존재하지 않는다면 가입 절차 진행.
            var newUser = new DBUser
            {
                _id = userName,
                _pw = encrytedUserPw,
                UId = DateTime.Now.Ticks
            };

            var collection = GetCollection<DBUser>(UserDbName, collectionName);
            try
            {
                await collection.InsertOneAsync(newUser);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return ErrorCode.MongoDBAddError;
            }

            return ErrorCode.None;
        }
    }
}

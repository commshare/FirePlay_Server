using MongoDB.Driver;

namespace DBServer.MongoDB
{
    class MongoDBManager
    {
        private const string ConnectionString = "mongodb://localhost:27017/?maxPoolSize=200";

        private const string UserDbName = "UserDB";

        public const string LoginCollectionName = "UserLoginInfo";

        public const string ScoreCollectionName = "UserScoreInfo";

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
    }
}

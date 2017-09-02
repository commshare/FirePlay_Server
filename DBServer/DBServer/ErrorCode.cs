
namespace DBServer
{
    public enum ErrorCode
    {
        None = 0,

        SignInIdAlreadExist = 100,

        ValidationInvalidId = 200,
        ValidationInvalidPw = 201,

        LogoutInvalidId = 300,
        LogoutInvalidToken = 301,

        MongoDBFindError = 400,

        RedisStartException = 500,
        RedisInvalidAddressString = 501,
        RedisStartSetTestFailed = 502
    }
}

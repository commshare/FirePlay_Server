

namespace LoginServer
{
	public enum ErrorCode
	{
		None = 0,

        // 600번대는 Login Server 에러 코드.
        SignInInvalidId = 610,

        // 700번대는 MongoDB 관련 에러 코드.
        MongoDBFindError = 700,
        MongoDBAddError = 701,

        LoginUserInfoDontExist = 710,
        LoginUserInfoAlreadyExist = 711,

        // 800번대는 Redis 관련 에러 코드.
        RedisStartException = 800,
        RedisInvalidAddressString = 801,
        RedisStartSetTestFailed = 802,

        RedisUnRegistedId = 810,
        RedisInvalidToken = 811,

        RedisTokenRegistError = 820,
        RedisTokenDeleteError = 821
    }
}
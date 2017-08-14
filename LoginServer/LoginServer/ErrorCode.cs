

namespace LoginServer
{
	public enum ErrorCode
	{
		None = 0,

		SignInIdAlreadyExist = 100,

		ValidationInvalidId = 200,
		ValidationInvalidPw = 201,

		LogoutInvalidId = 300,
		LogoutInvalidToken = 301,

		MongoDbFindError = 400,

		RedisStartException = 500,
		RedisInvalidAddressString = 501,
		RedisStartSetTestFailed = 502,
	}
}
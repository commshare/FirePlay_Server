

namespace LoginServer
{
	public enum ErrorCode
	{
		None = 0,

		ReqLoginIdAlreadyExist = 10,
		ReqLoginInvalidId = 11,
		ReqLoginInvalidPw = 12,

		MongoDbFindError = 100,

		RedisStartException = 200,
		RedisInvalidAddressString = 201,
		RedisStartSetTestFailed = 202,
	}
}
#pragma once

namespace FPCommon
{
	enum class ErrorCode : short
	{
		None = 0,

		FailConfigLoad = 10,

		// 100번대는 Login 관련 에러 코드.
		NoRoomInUserPool = 100,

		// 200번대는 Game 관련 에러 코드.
		InvalidRoomNumber = 200,

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
	};
}
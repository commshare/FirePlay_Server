#pragma once

namespace FPCommon
{
	enum class ErrorCode : short
	{
		None = 0,

		FailConfigLoad = 10,

		// 100����� Login ���� ���� �ڵ�.
		NoRoomInUserPool = 100,

		// 200����� Game ���� ���� �ڵ�.
		InvalidRoomNumber = 200,

		// 600����� Login Server ���� �ڵ�.
		SignInInvalidId = 610,

		// 700����� MongoDB ���� ���� �ڵ�.
		MongoDBFindError = 700,
		MongoDBAddError = 701,

		LoginUserInfoDontExist = 710,
		LoginUserInfoAlreadyExist = 711,

		// 800����� Redis ���� ���� �ڵ�.
		RedisStartException = 800,
		RedisInvalidAddressString = 801,
		RedisStartSetTestFailed = 802,

		RedisUnRegistedId = 810,
		RedisInvalidToken = 811,

		RedisTokenRegistError = 820,
		RedisTokenDeleteError = 821
	};
}
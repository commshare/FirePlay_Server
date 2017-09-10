#pragma once
#include <unordered_map>

#include "../../Common/ObjectPool.h"

#include "User.h"

namespace FPLogic
{
	using UserPool = FPCommon::ObjectPool<User>;

	class UserManager
	{
	public :

		UserManager() {}
		~UserManager() {}

		void Init(const int maxUserCount);

		// 유저를 로그인 시켜주는 메소드.
		bool LoginUser(const int sessionIdx, std::string id, std::string token);

		// 유저를 찾아주는 메소드.
		User * FindUserWithSessionIdx(const int sessionIdx);

	private :

		// @ Key : Session Idx
		// @ Value : User Pool Idx
		std::unordered_map<int, int> _userSessionDic;

		UserPool _userPool;
	};
}

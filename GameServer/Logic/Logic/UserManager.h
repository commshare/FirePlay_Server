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

		// ������ �α��� �����ִ� �޼ҵ�.
		bool LoginUser(const int sessionIdx, std::string id, std::string token);

		// ������ ã���ִ� �޼ҵ�.
		User * FindUserWithSessionIdx(const int sessionIdx);

	private :

		// @ Key : Session Idx
		// @ Value : User Pool Idx
		std::unordered_map<int, int> _userSessionDic;

		UserPool _userPool;
	};
}

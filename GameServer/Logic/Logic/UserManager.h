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


	private :

		UserPool _userPool;
	};
}

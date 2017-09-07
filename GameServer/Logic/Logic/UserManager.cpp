#include "UserManager.h"

#include "User.h"

namespace FPLogic
{
	void UserManager::Init(const int maxUserCount)
	{
		_userPool.Init(maxUserCount);
	}

	bool UserManager::LoginUser(const int sessionIdx, std::string id, std::string token)
	{
		auto newUserTag = _userPool.GetTag();
		// 유저 풀에 자리가 없는 경우.
		if (newUserTag == -1)
		{
			return false;
		}

		auto newUser = _userPool[newUserTag];
		newUser.Clear();
		newUser.Login(sessionIdx, id.c_str());
		return true;
	}
}

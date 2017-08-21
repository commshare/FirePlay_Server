#include "UserManager.h"

namespace FPLogic
{
	void UserManager::Init(const int maxUserCount)
	{
		_userPool.Init(maxUserCount);
	}
}

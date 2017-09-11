#include "GameManager.h"

namespace FPLogic
{
	void GameManager::Init()
	{
		_gameRoomPool.Init(maxGameRoomNum);
	}

}
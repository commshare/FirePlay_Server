#include "GameRoomManager.h"

namespace FPLogic
{
	void GameRoomManager::Init()
	{
		_gameRoomPool.Init(maxGameRoomNum);
	}

}
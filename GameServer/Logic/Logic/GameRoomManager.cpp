#include "GameRoomManager.h"

namespace FPLogic
{
	void GameRoomManager::Init(ConsoleLogger * logger, PacketQueue * sendQueue, UserManager * userManager)
	{
		_logger = logger;
		_sendQueue = sendQueue;
		_userManager = userManager;

		_gameRoomPool.Init(maxGameRoomNum);
	}

	// GameRoom 오브젝트 풀에서 빈 방을 찾아 반환하는 메소드.
	int GameRoomManager::GetEmptyRoom()
	{
		return _gameRoomPool.GetTag();
	}

}
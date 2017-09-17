#pragma once

#include "../../Common/ObjectPool.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/PacketQueue.h"

#include "GameRoom.h"
#include "User.h"
#include "UserManager.h"

namespace FPLogic
{
	using GameRoomPool = FPCommon::ObjectPool<GameRoom>;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using PacketQueue = FPCommon::PacketQueue;

	const int maxGameRoomNum = 200;

	class GameRoomManager
	{
	public :
		GameRoomManager() = default;
		~GameRoomManager() {}

		void Init(ConsoleLogger * logger, PacketQueue * sendQueue, UserManager * userManager);

		void UpdateRooms();

		int GetEmptyRoom();

		void EnterUserToRoom(const int sessionId, const int gameIdx);

	private :
		
		GameRoomPool    _gameRoomPool;
		ConsoleLogger * _logger;
		UserManager   * _userManager;
		PacketQueue   * _sendQueue;
	};

}
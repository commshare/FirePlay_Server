#pragma once

#include "../../Common/ObjectPool.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/ErrorCode.h"

#include "GameRoom.h"
#include "User.h"
#include "UserManager.h"

namespace FPLogic
{
	using GameRoomPool = FPCommon::ObjectPool<GameRoom>;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using PacketQueue = FPCommon::PacketQueue;
	using ErrorCode = FPCommon::ErrorCode;
	using LogType = FPCommon::LogType;

	const int maxGameRoomNum = 200;

	class GameRoomManager
	{
	public :
		GameRoomManager() = default;
		~GameRoomManager() {}

		void Init(ConsoleLogger * logger, PacketQueue * sendQueue, UserManager * userManager);

		void UpdateRooms();

		int GetEmptyRoom();

		GameRoom* GetRoom(const int roomNumber);

		ErrorCode EnterUserToRoom(const int sessionId, const int gameRoomIdx);

		ErrorCode GameStartAck(const int roomNumber);

		ErrorCode TurnChange(const int roomNumber);

		void SetDamageInfo(const int roomNumber, const int player1Hp, const int player2Hp);

	private :

		void roomProcess(GameRoom * room);
		
		GameRoomPool    _gameRoomPool;
		ConsoleLogger * _logger;
		UserManager   * _userManager;
		PacketQueue   * _sendQueue;
	};

}
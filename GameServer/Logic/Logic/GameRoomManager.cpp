#include "GameRoomManager.h"

#include "../../Common/Util.h"

namespace FPLogic
{
	using Util = FPCommon::Util;

	void GameRoomManager::Init(ConsoleLogger * logger, PacketQueue * sendQueue, UserManager * userManager)
	{
		_logger = logger;
		_sendQueue = sendQueue;
		_userManager = userManager;

		_gameRoomPool.Init(maxGameRoomNum);
	}

	// TODO :: 여기서 룸 상태 체크, 프로세스를 진행해주어야 한다.
	void GameRoomManager::UpdateRooms()
	{
		auto countActivateRoom = 0;
		// TODO :: 오브젝트 풀 클래스를 Range based for Loop을 지원할 수 있도록.
		for (auto i = 0; i < _gameRoomPool.GetSize(); ++i)
		{
			// 활성화된 룸 개수를 세어서 전체를 다 돌지 않아도 되도록
			if (countActivateRoom >= _gameRoomPool.GetActivatedObjectCount())
			{
				break;
			}

			auto room = &_gameRoomPool[i];

			if (room->GetState() == RoomState::None)
			{
				continue;
			}

			roomProcess(room);
			++countActivateRoom;
		}
	}

	// GameRoom 오브젝트 풀에서 빈 방을 찾아 반환하는 메소드.
	int GameRoomManager::GetEmptyRoom()
	{
		return _gameRoomPool.GetTag();
	}

	GameRoom * GameRoomManager::GetRoom(const int roomNumber)
	{
		return &_gameRoomPool[roomNumber];
	}

	ErrorCode GameRoomManager::EnterUserToRoom(const int sessionId, const int gameRoomIdx)
	{
		auto& gameRoom = _gameRoomPool[gameRoomIdx];
		auto enteringUser = _userManager->FindUserWithSessionIdx(sessionId);

		return gameRoom.EnterUser(enteringUser);
	}

	ErrorCode GameRoomManager::GameStartAck(const int roomNumber)
	{
		auto room = &_gameRoomPool[roomNumber];

		room->AckGameStart();

		return ErrorCode::None;
	}

	ErrorCode GameRoomManager::TurnChange(const int roomNumber)
	{
		auto room = &_gameRoomPool[roomNumber];

		if (room == nullptr)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Invalid TurnChange Func call, Room Number(%d)", __FUNCTION__, roomNumber);
			return ErrorCode::InvalidRoomNumber;
		}

		Packet::TurnStartNotify turnStartNotify;
		Packet::EnemyTurnStartNotify enemyTurnStartNotify;

		if (room->_turnPlayer == 1)
		{
			room->_turnPlayer = 2;

			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_TurnStartNotify, room->_player2->GetSessionIdx(), &turnStartNotify);
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_EnemyTurnStartNotify, room->_player1->GetSessionIdx(), &enemyTurnStartNotify);
		}
		else
		{
			room->_turnPlayer = 1;

			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_TurnStartNotify, room->_player1->GetSessionIdx(), &turnStartNotify);
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_EnemyTurnStartNotify, room->_player2->GetSessionIdx(), &enemyTurnStartNotify);
		}

		return ErrorCode::None;
	}

	void GameRoomManager::roomProcess(GameRoom * room)
	{
		switch (room->GetState())
		{
		case RoomState::Waiting:
			#pragma region WAITING ROOM PROCESS

			// 방에 참가자가 다 차있다면, 게임을 시작을 알리는 패킷을 보낸다.
			if (room->GetPlayerCount() >= 2 && room->_isGameStartPacketSended == false)
			{
				// 플레이어 포지션 랜덤 생성.
				//auto player1Pos = Util::GetRandomNumber(-100, 100);
				//auto player2Pos = Util::GetRandomNumber(-100, 100);
				auto player1Pos = 10;
				auto player2Pos = -10;

				// 패킷에 플레이어 정보 기록.
				Packet::GameStartNotify notify1;
				notify1._playerNumber = 1;
				notify1._positionX = player1Pos;
				notify1._enemyPositionX = player2Pos;

				Packet::GameStartNotify notify2;
				notify2._playerNumber = 2;
				notify2._positionX = player2Pos;
				notify2._enemyPositionX = player1Pos;

				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_GameStartNotify, room->_player1->GetSessionIdx(), &notify1);
				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_GameStartNotify, room->_player2->GetSessionIdx(), &notify2);

				// 생성한 정보를 방에 기록.
				room->_player1Pos = player1Pos;
				room->_player2Pos = player2Pos;

				room->_isGameStartPacketSended = true;

				_logger->Write(LogType::LOG_DEBUG, "%s | Player 1 Position(%d), 2 Position(%d)", __FUNCTION__, player1Pos, player2Pos);
			}

			#pragma endregion
			break;

		case RoomState::StartGame:
			#pragma region START GAME
		{
			// 턴을 정한다.
			auto turnCoin = Util::GetRandomNumber(0, 100);

			// 보낼 패킷을 만들어 놓는다.
			// TODO :: 바람 만들어서 보내기.
			Packet::TurnStartNotify turnStartNotify;
			Packet::EnemyTurnStartNotify enemyTurnStartNotify;

			// 정한 턴을 기록하고, 각자에게 패킷을 보내준다.
			if (turnCoin >= 50)
			{
				room->_turnPlayer = 1;

				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_TurnStartNotify, room->_player1->GetSessionIdx(), &turnStartNotify);
				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_EnemyTurnStartNotify, room->_player2->GetSessionIdx(), &enemyTurnStartNotify);
			}
			else
			{
				room->_turnPlayer = 2;

				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_TurnStartNotify, room->_player2->GetSessionIdx(), &turnStartNotify);
				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_EnemyTurnStartNotify, room->_player1->GetSessionIdx(), &enemyTurnStartNotify);
			}

			// 방 상태를 InGame으로 바꿔준다.
			room->_state = RoomState::InGame;

		}
			#pragma endregion
			break;

		case RoomState::InGame:
			#pragma region INGAME ROOM PROCESS

			// TODO :: 타이머 클래스 만든 후 시간 재서 턴 끝내기.

			#pragma endregion
			break;

		case RoomState::EndGame:
			#pragma region ENDGAME ROOM PROCESS

			#pragma endregion
			break;
		}
	}
}
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

	ErrorCode GameRoomManager::EnterUserToRoom(const int sessionId, const int gameRoomIdx)
	{
		auto& gameRoom = _gameRoomPool[gameRoomIdx];
		auto enteringUser = _userManager->FindUserWithSessionIdx(sessionId);

		return gameRoom.EnterUser(enteringUser);
	}

	bool GameRoomManager::roomProcess(GameRoom * room)
	{
		switch (room->GetState)
		{
		case RoomState::Waiting:
			#pragma region WAITING ROOM PROCESS
			// 방에 참가자가 다 차있다면, 게임을 시작을 알리는 패킷을 보낸다.
			if (room->GetPlayerCount() >= 2 && room->_isGameStartPacketSended == false)
			{
				// 플레이어 포지션 랜덤 생성.
				auto player1Pos = Util::GetRandomNumber(-100, 100);
				auto player2Pos = Util::GetRandomNumber(-100, 100);

				// 패킷에 플레이어 정보 기록.
				Packet::GameStartNotify notify1;
				notify1._playerNumber = 1;
				notify1._positionX = player1Pos;
				notify1._enemyPositionX = player2Pos;
				notify1._positionY = 300;
				notify1._enemyPositionY = 300;

				Packet::GameStartNotify notify2;
				notify2._playerNumber = 2;
				notify2._positionX = player2Pos;
				notify2._enemyPositionX = player1Pos;
				notify2._positionY = 300;
				notify2._enemyPositionY = 300;

				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_GameStartNotify, room->_player1->GetSessionIdx(), &notify1);
				Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_GameStartNotify, room->_player2->GetSessionIdx(), &notify2);

				room->_isGameStartPacketSended = true;
			}

			#pragma endregion
			break;

		case RoomState::InGame:
			#pragma region INGAME ROOM PROCESS

			room->GameProcess();

			#pragma endregion
			break;

		case RoomState::EndGame:
			#pragma region ENDGAME ROOM PROCESS

			room->EndGame();

			#pragma endregion
			break;
		}
	}

}
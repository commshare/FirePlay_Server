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

			// 방에 참가자가 다 차있다면, 게임을 시작시킨다.
			if (room->GetPlayerCount() >= 2)
			{
				room->StartGame();
			}

			#pragma endregion
			break;

		case RoomState::InGame:
			#pragma region INGAME ROOM PROCESS

			#pragma endregion
			break;

		case RoomState::EndGame:
			#pragma region ENDGAME ROOM PROCESS

			#pragma endregion
			break;
		}
	}

}
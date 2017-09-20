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

	// TODO :: ���⼭ �� ���� üũ, ���μ����� �������־�� �Ѵ�.
	void GameRoomManager::UpdateRooms()
	{
		auto countActivateRoom = 0;
		// TODO :: ������Ʈ Ǯ Ŭ������ Range based for Loop�� ������ �� �ֵ���.
		for (auto i = 0; i < _gameRoomPool.GetSize(); ++i)
		{
			// Ȱ��ȭ�� �� ������ ��� ��ü�� �� ���� �ʾƵ� �ǵ���
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

	// GameRoom ������Ʈ Ǯ���� �� ���� ã�� ��ȯ�ϴ� �޼ҵ�.
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

			// �濡 �����ڰ� �� ���ִٸ�, ������ ���۽�Ų��.
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
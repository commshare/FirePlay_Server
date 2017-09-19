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

			if (roomProcess(room))
			{
				++countActivateRoom;
			}
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
		// TODO :: �� ���¿� ���� �ٸ� ������ ���ư� �� �ֵ���.
		switch (room->GetState)
		{
		case RoomState::Waiting:

			return false;

		case RoomState::InGame:

			return true;

		case RoomState::EndGame:

			return true;
		}
	}

}
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
			// �濡 �����ڰ� �� ���ִٸ�, ������ ������ �˸��� ��Ŷ�� ������.
			if (room->GetPlayerCount() >= 2 && room->_isGameStartPacketSended == false)
			{
				// �÷��̾� ������ ���� ����.
				auto player1Pos = Util::GetRandomNumber(-100, 100);
				auto player2Pos = Util::GetRandomNumber(-100, 100);

				// ��Ŷ�� �÷��̾� ���� ���.
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
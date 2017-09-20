#pragma once
#include <memory>

#include "../../Common/ErrorCode.h"

#include "User.h"

namespace FPLogic
{
	using ErrorCode = FPCommon::ErrorCode;

	enum class RoomState : int
	{
		None,
		Waiting,
		InGame,
		EndGame
	};

	class GameRoom
	{
	public :

		GameRoom() {}
		~GameRoom() {}

		RoomState GetState() const { return _state; }

		void Clear()
		{
			_state = RoomState::None;
			_player1 = nullptr;
			_player2 = nullptr;
			_playerCount = 0;

			_isGameStartPacketSended = false;
		}

		ErrorCode EnterUser(User * enteringUser)
		{
			if (_playerCount >= 2)
			{
				return ErrorCode::RoomAlreadyFull;
			}

			if (_player1 = nullptr)
			{
				_player1 = enteringUser;
			}
			else
			{
				_player2 = enteringUser;
			}

			++_playerCount;
			_state = RoomState::Waiting;
		}

		int GetPlayerCount() const { return _playerCount; }

		void GameProcess()
		{

		}

		void EndGame()
		{

		}

		RoomState _state;
		int _playerCount = 0;
		User * _player1 = nullptr;
		User * _player2 = nullptr;

		bool _isGameStartPacketSended = false;
	};
}

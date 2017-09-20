#pragma once
#include <memory>

#include "User.h"
#include "../../Common/ErrorCode.h"

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

		void StartGame()
		{

		}

		int GetPlayerCount() const { return _playerCount; }

	private :

		RoomState _state;
		int _playerCount = 0;
		User * _player1 = nullptr;
		User * _player2 = nullptr;
	};
}

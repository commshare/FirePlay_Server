#pragma once
#include <memory>

#include "User.h"
#include "../../Common/ErrorCode.h"

namespace FPLogic
{
	using ErrorCode = FPCommon::ErrorCode;

	enum class RoomState : int
	{
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
			_state = RoomState::Waiting;
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
		}

	private :

		RoomState _state;
		int _playerCount = 0;
		User * _player1 = nullptr;
		User * _player2 = nullptr;
	};
}

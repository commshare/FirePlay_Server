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
		StartGame,
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
			_player1Pos = 0;
			_player2Pos = 0;
			_playerCount = 0;
			_ackCount = 0;

			_isGameStartPacketSended = false;
			_isGameStartReadyed = false;
			_turnPlayer = 0;
		}

		ErrorCode EnterUser(User * enteringUser)
		{
			if (_playerCount >= 2)
			{
				return ErrorCode::RoomAlreadyFull;
			}

			if (_player1 == nullptr)
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

		void AckGameStart()
		{
			if (_ackCount >= 2)
			{
				++_ackCount;
			}

			if (_ackCount >= 2)
			{
				_state = RoomState::StartGame;
			}
		}

		void PlayerMove(const int ackPlayerSessionId, const int moveDist)
		{
			if (ackPlayerSessionId == _player1->GetSessionIdx())
			{
				_player1Pos += moveDist;
			}
			else
			{
				_player2Pos += moveDist;
			}
		}

		int GetAnotherPlayerSession(const int sessionId)
		{
			if (sessionId == _player1->GetSessionIdx())
			{
				return _player2->GetSessionIdx();
			}
			else
			{
				return _player1->GetSessionIdx();
			}
		}

		// RoomState
		RoomState _state;
		int _playerCount = 0;
		int _ackCount = 0;

		bool _isGameStartPacketSended = false;
		bool _isGameStartReadyed = false;

		// Player
		User * _player1 = nullptr;
		User * _player2 = nullptr;
		int _player1Pos = 0;
		int _player2Pos = 0;

		// For Logic
		int _turnPlayer = 0;
	};
}

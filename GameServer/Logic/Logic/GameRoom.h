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
			_player1Hp = -1;
			_player2Hp = -1;
			_playerCount = 0;
			_ackCount = 0;

			_isGameStartPacketSended = false;
			_isGameStartReadyed = false;
			_turnPlayer = 0;
			_winPlayer = 0;
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
			if (_ackCount <= 2)
			{
				++_ackCount;
			}

			if (_ackCount >= 2)
			{
				_state = RoomState::StartGame;
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

		void InitializePlayer()
		{
			// TODO :: 좋지 못한 코드임.
			// 추후에 무조건 고쳐야 함.
			if (_player1->GetCharacterType() == CharacterType::Archer1)
			{
				_player1Hp = 80;
			}
			else if (_player1->GetCharacterType() == CharacterType::Archer2)
			{
				_player1Hp = 100;
			}
			else
			{
				_player1Hp = 120;
			}

			if (_player2->GetCharacterType() == CharacterType::Archer1)
			{
				_player2Hp = 80;
			}
			else if (_player2->GetCharacterType() == CharacterType::Archer2)
			{
				_player2Hp = 100;
			}
			else
			{
				_player2Hp = 120;
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
		int _player1Hp = -1;
		int _player2Hp = -1;

		// For Logic
		int _turnPlayer = 0;
		int _winPlayer = 0;
	};
}

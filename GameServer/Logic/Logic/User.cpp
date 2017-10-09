#include "User.h"

#include "Define.h"

namespace FPLogic
{
	void User::Init(const int idx)
	{
		_idx = idx;
	}
	
	void User::Clear()
	{
		_idx = 0;

		_sessionIdx = 0;

		_gameIdx = 0;

		_selectedCharacterType = CharacterType::None;

		_id.clear();

		_isUserActivated = false;

		_state = UserState::None;

		_wins = 0;

		_loses = 0;
	}

	void User::Login(const int sessionIdx, const char * id)
	{
		_sessionIdx = sessionIdx;
		_id = id;

		_isUserActivated = true;
		_state = UserState::Waiting;
	}

	void User::JoinMatching(CharacterType characterType, MatchingType matchingType)
	{
		// MatchingType은 현재는 고려하지 않음.
		_selectedCharacterType = characterType;
		_state = UserState::Matching;
	}

	void User::JoinGame(const int gameIdx)
	{
		_gameIdx = gameIdx;
		_state = UserState::InGame;
	}

	void User::CancelMatching()
	{
		_selectedCharacterType = CharacterType::None;
		_state = UserState::Waiting;
	}

	void User::LeaveGame()
	{
		_selectedCharacterType = CharacterType::None;
		_state = UserState::Waiting;
	}
}

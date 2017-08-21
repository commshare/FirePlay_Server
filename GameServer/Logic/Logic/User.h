#pragma once
#include <string>

#include "Define.h"

namespace FPLogic
{
	class User
	{
	public :

		enum class UserState : short
		{
			None = 0,
			Waiting = 1,	
			Matching = 2,
			InGame = 3
		};

		User() {}
		~User() {}

		void Init(const int idx);
		void Clear();

		void Login(const int sessionIdx, const char * id);
		void JoinMatching(CharacterType characterType, MatchingType matchingType);
		void JoinGame(const int gameIdx);
		
		void CancelMatching();
		void LeaveGame();

		// Getter
		int           GetIdx()           const { return _idx;                   }
		int           GetSessionIdx()    const { return _sessionIdx;            }
		int           GetGameIdx()       const { return _gameIdx;               }
		CharacterType GetCharacterType() const { return _selectedCharacterType; }
		std::string   GetId()                  { return _id;                    }
		bool		  IsUserActivated()  const { return _isUserActivated;       }
		int           GetRating()        const { return _rating;                }
		UserState     GetUserState()     const { return _state;                 }

	private :

		int _idx = 0;

		int _sessionIdx = 0;

		int _gameIdx = 0;

		CharacterType _selectedCharacterType = CharacterType::None;

		std::string _id;

		bool _isUserActivated = false;

		UserState _state = UserState::None;

		int _rating = 0;
	};
}

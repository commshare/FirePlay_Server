#pragma once
#include <string>

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

		void Clear();

	private :

		int _idx = 0;

		int _sessionIdx = 0;

		std::string _id;

		bool _isUserActivated = false;

		UserState _state = UserState::None;

		int _rating = 0;
	};
}

#include "User.h"

namespace FPLogic
{
	void User::Clear()
	{
		_idx = 0;

		_sessionIdx = 0;

		_id.clear();

		_isUserActivated = false;

		_state = UserState::None;

		_rating = 0;
	}
}

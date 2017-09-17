#pragma once
#include "User.h"

namespace FPLogic
{
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

	private :

		RoomState _state;
		
	};
}

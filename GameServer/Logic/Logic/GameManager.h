#pragma once

#include "../../Common/ObjectPool.h"

#include "GameRoom.h"

namespace FPLogic
{
	using GameRoomPool = FPCommon::ObjectPool<GameRoom>;

	const int maxGameRoomNum = 200;

	class GameManager
	{
	public :
		GameManager() = default;
		~GameManager() {}

		void Init();

	private :
		
		GameRoomPool _gameRoomPool;
	};

}
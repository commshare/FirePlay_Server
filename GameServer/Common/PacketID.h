#pragma once

namespace FPCommon
{
	enum class PacketId : short
	{
		LoginReq = 21,
		LoginRes = 22,

		FastMatchStartReq = 31,
		FastMatchStartRes = 32,

		NotifyMatchSuccess = 41,
		AckMatchSuccess = 42,

		NotifyGameStart = 101,
		AckGameStart = 102,

		NotifyTurnStart = 111,
		AckTurnStart = 112,

		NotifyEnemyTurnStart = 121,
		AckEnemyTurnStart = 122,

		NotifyMove = 131,
		NotifyEnemyMove = 133,

		NotifyFire = 141,
		NotifyEnemyFire = 142,

	};
}
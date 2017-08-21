#pragma once

namespace FPLogic
{
	enum class CharacterType
	{
		None = 0,
		Archer1 = 1,
		Archer2 = 2,
		Archer3 = 3
	};

	// TODO :: 우선 패스트 매치만 구현.
	enum class MatchingType
	{
		None = 0,
		FastMatch = 1,
		NumberMatch = 2
	};
}

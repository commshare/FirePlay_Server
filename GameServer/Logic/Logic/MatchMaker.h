#pragma once
#include <deque>

#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"

namespace FPLogic
{
	using PacketQueue = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;

	class MatchMaker
	{
	public :

		MatchMaker() {}
		~MatchMaker() {}
		void Init(ConsoleLogger * logger, PacketQueue * sendQueue)
		{
			_logger = logger;
			_sendQueue = sendQueue;
		}

		void CheckMatchMaked()
		{
			// TODO :: 두 명 이상 기다리면 바로 매칭을 시켜주게 되는 임시 코드.
			if (_matchingQueue.size() >= 2)
			{
				// 매칭이 되었다는 패킷을 보내준다.
				// 이 두사람을 매칭 큐에서 빼준다.
			}
		}

		// RequestMatch : 매칭 큐에 집어 넣어 매칭 상대를 요청하는 메소드.
		// TODO :: 일단 기다리는 사람이 두 명 이상이면 무조건 매칭.
		// 나중에 매칭 랭킹 시스템 도입.
		void RequestMatch(const int sessionId)
		{
			_matchingQueue.push_back(sessionId);
		}

	private :

		// TODO :: 게임 인덱스는 어떻게 관리하지?
		void SendMatchingPacket(const int sessionIdx, const int gameIdx)
		{

		}

		// 매칭을 기다리는 큐를 가지고 있음.
		// TODO :: 전용 자료구조를 만들어주는 것이 좋을 것 같다.
		// 지금은 시간이 없어서 deque로 대체.
		std::deque<int> _matchingQueue;

		ConsoleLogger * _logger;
		PacketQueue * _sendQueue;
	};
}

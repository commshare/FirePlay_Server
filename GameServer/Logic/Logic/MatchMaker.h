#pragma once
#include <deque>
#include <algorithm>

#include "../../Common/Packet.h"

#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"

#include "GameRoomManager.h"

namespace FPLogic
{
	using PacketQueue = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;

	class MatchMaker
	{
	public :

		MatchMaker() {}
		~MatchMaker() {}
		void Init(ConsoleLogger * logger, PacketQueue * sendQueue, GameRoomManager * gameRoomManager)
		{
			_logger = logger;
			_sendQueue = sendQueue;
			_gameRoomManager = gameRoomManager;
		}

		void CheckMatchMaked()
		{
			// TODO :: �� �� �̻� ��ٸ��� �ٷ� ��Ī�� �����ְ� �Ǵ� �ӽ� �ڵ�.
			if (_matchingQueue.size() >= 2)
			{
				// �� �λ���� ��Ī ť���� ���ش�.
				auto player1Idx = _matchingQueue.front();
				_matchingQueue.pop_front();
				auto player2Idx = _matchingQueue.front();
				_matchingQueue.pop_front();

				// ��Ī�� �Ǿ��ٴ� ��Ŷ�� �����ش�.
				Packet::MatchSuccessNotify notifyPacketToPlayer1;
				Packet::MatchSuccessNotify notifyPacketToPlayer2;

			}
		}

		// RequestMatch : ��Ī ť�� ���� �־� ��Ī ��븦 ��û�ϴ� �޼ҵ�.
		// TODO :: �ϴ� ��ٸ��� ����� �� �� �̻��̸� ������ ��Ī.
		// ���߿� ��Ī ��ŷ �ý��� ����.
		void RequestMatch(const int sessionId)
		{
			_matchingQueue.push_back(sessionId);
		}

		bool isUserIsInMatching(const int sessionId)
		{
			auto findResult = std::find_if(_matchingQueue.begin(), _matchingQueue.end(), sessionId);
			if (findResult == _matchingQueue.end())
			{
				return false;
			}
			return true;
		}

	private :

		// TODO :: ���� �ε����� ��� ��������?
		void SendMatchingPacket(const int sessionIdx, const int gameIdx)
		{

		}

		// ��Ī�� ��ٸ��� ť�� ������ ����.
		// TODO :: ���� �ڷᱸ���� ������ִ� ���� ���� �� ����.
		// ������ �ð��� ��� deque�� ��ü.
		std::deque<int> _matchingQueue;

		ConsoleLogger * _logger;
		PacketQueue * _sendQueue;
		GameRoomManager * _gameRoomManager;
	};
}

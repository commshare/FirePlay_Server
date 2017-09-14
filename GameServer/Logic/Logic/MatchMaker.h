#pragma once
#include <deque>
#include <memory>
#include <algorithm>

#include "../../Common/Packet.h"

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
			// TODO :: �� �� �̻� ��ٸ��� �ٷ� ��Ī�� �����ְ� �Ǵ� �ӽ� �ڵ�.
			if (_matchingQueue.size() >= 2)
			{
				// �� �λ���� ��Ī ť���� ���ش�.
				auto player1Idx = _matchingQueue.front();
				_matchingQueue.pop_front();
				auto player2Idx = _matchingQueue.front();
				_matchingQueue.pop_front();

				// TODO :: ��Ī�� �Ǿ��ٴ� ��Ŷ�� �����ش�.
				Packet::MatchSuccessNotify notifyPacketToPlayer1;
				Packet::MatchSuccessNotify notifyPacketToPlayer2;


			}
		}

		// RequestMatch : ��Ī ť�� ���� �־� ��Ī ��븦 ��û�ϴ� �޼ҵ�.
		// TODO :: �ϴ� ��ٸ��� ����� �� �� �̻��̸� ������ ��Ī.
		// ���߿� ��Ī ��ŷ �ý��� ����.
		void RequestMatch(const int sessionId)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_matchingQueue.push_back(sessionId);
		}

		// ��ġ�� ��û�� ������ ������� ��츦 ó�����ִ� �޼ҵ�. 
		void CancleMatch(const int sessionId)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			// ��û���� ������ ã�� �����Ѵ�.
			_matchingQueue.erase(std::remove_if(_matchingQueue.begin(), _matchingQueue.end(), [](int sessionId) {}), _matchingQueue.end());
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
		std::mutex _mutex;

		ConsoleLogger * _logger;
		PacketQueue * _sendQueue;
	};
}

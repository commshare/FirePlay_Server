#pragma once
#include <deque>

#include "../../Common/PacketQueue.h"

namespace FPLogic
{
	using PacketQueue = FPCommon::PacketQueue;

	class MatchMaker
	{
	public :

		MatchMaker() {}
		~MatchMaker() {}
		void Init(PacketQueue * sendQueue)
		{
			_sendQueue = sendQueue;
		}

		void CheckMatchMaked()
		{
			// TODO :: �� �� �̻� ��ٸ��� �ٷ� ��Ī�� �����ְ� �Ǵ� �ӽ� �ڵ�.
			if (_matchingQueue.size() >= 2)
			{
				// ��Ī�� �Ǿ��ٴ� ��Ŷ�� �����ش�.
				// �� �λ���� ��Ī ť���� ���ش�.
			}
		}

		// RequestMatch : ��Ī ť�� ���� �־� ��Ī ��븦 ��û�ϴ� �޼ҵ�.
		// TODO :: �ϴ� ��ٸ��� ����� �� �� �̻��̸� ������ ��Ī.
		// ���߿� ��Ī ��ŷ �ý��� ����.
		void RequestMatch(const int sessionId)
		{
			_matchingQueue.push_back(sessionId);
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

		PacketQueue * _sendQueue;
	};
}

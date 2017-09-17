#pragma once
#include <deque>
#include <memory>
#include <algorithm>

#include "../../Common/Packet.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/Util.h"

#include "GameRoomManager.h"
#include "UserManager.h"
#include "User.h"

namespace FPLogic
{
	using PacketQueue = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using Util = FPCommon::Util;

	class MatchMaker
	{
	public :

		MatchMaker() {}
		~MatchMaker() {}
		void Init(ConsoleLogger * logger, PacketQueue * sendQueue, GameRoomManager * gameRoomManager, UserManager * userManager)
		{
			_logger = logger;
			_sendQueue = sendQueue;
			_gameRoomManager = gameRoomManager;
			_userManager = userManager;
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

				auto gameRoomIdx = _gameRoomManager->GetEmptyRoom();

				// ���� ���ٸ� ��Ī ť�� �ٽ� ����ִ´�.
				if (gameRoomIdx == -1)
				{
					_matchingQueue.push_front(player2Idx);
					_matchingQueue.push_front(player1Idx);
				}
				else
				{
					SendMatchingPacket(player1Idx, player2Idx, gameRoomIdx);
				}
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

		bool isUserIsInMatching(const int sessionId)
		{
			auto findResult = std::find_if(_matchingQueue.begin(), _matchingQueue.end(), sessionId);
			if (findResult == _matchingQueue.end())
			{
				return false;
			}
			return true;
		}

		// ��ġ�� ��û�� ������ ������� ��츦 ó�����ִ� �޼ҵ�. 
		void CancleMatch(const int sessionId)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			// ��û���� ������ ã�� �����Ѵ�.
			std::remove_if(_matchingQueue.begin(), _matchingQueue.end(), sessionId);
		}

	private :

		void SendMatchingPacket(const int player1Idx, const int player2Idx, const int gameIdx)
		{
			auto user1 = _userManager->FindUserWithSessionIdx(player1Idx);
			auto user2 = _userManager->FindUserWithSessionIdx(player2Idx);

			Packet::MatchSuccessNotify matchNotify1;
			Packet::MatchSuccessNotify matchNotify2;

			matchNotify1._enemyId = user2->GetId();
			matchNotify1._enemyType = static_cast<int>(user2->GetCharacterType());

			// TODO :: ��� ���� �� �� �ֵ���.
			matchNotify1._enemyWins = 0;
			matchNotify1._enemyLoses = 0;

			matchNotify1._gameNumber = gameIdx;

			matchNotify2._enemyId = user1->GetId();
			matchNotify2._enemyType = static_cast<int>(user1->GetCharacterType());

			// TODO :: ���������� ��� ���� �� �� �ֵ���.
			matchNotify2._enemyWins = 0;
			matchNotify2._enemyLoses = 0;

			matchNotify2._gameNumber = gameIdx;

			// WARN :: Util Ŭ������ PushToSendQueue�� _sendQueue�� ���ڷ� �ޱ� ������ ������ ���� ����.
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_MatchSuccessNotify, user1->GetSessionIdx(), &matchNotify1);
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_MatchSuccessNotify, user2->GetSessionIdx(), &matchNotify2);
		}

		// ��Ī�� ��ٸ��� ť�� ������ ����.
		// TODO :: ���� �ڷᱸ���� ������ִ� ���� ���� �� ����.
		// ������ �ð��� ��� deque�� ��ü.
		std::deque<int>       _matchingQueue;
		std::mutex            _mutex;

		ConsoleLogger   *     _logger;
		PacketQueue     *     _sendQueue;
		GameRoomManager *     _gameRoomManager;
		UserManager		*	  _userManager;
	};
}

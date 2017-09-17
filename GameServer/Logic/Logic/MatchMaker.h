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
			// TODO :: 두 명 이상 기다리면 바로 매칭을 시켜주게 되는 임시 코드.
			if (_matchingQueue.size() >= 2)
			{
				// 이 두사람을 매칭 큐에서 빼준다.
				auto player1Idx = _matchingQueue.front();
				_matchingQueue.pop_front();
				auto player2Idx = _matchingQueue.front();
				_matchingQueue.pop_front();

				auto gameRoomIdx = _gameRoomManager->GetEmptyRoom();

				// 방이 없다면 매칭 큐에 다시 집어넣는다.
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

		// RequestMatch : 매칭 큐에 집어 넣어 매칭 상대를 요청하는 메소드.
		// TODO :: 일단 기다리는 사람이 두 명 이상이면 무조건 매칭.
		// 나중에 매칭 랭킹 시스템 도입.
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

		// 매치를 신청한 유저가 취소했을 경우를 처리해주는 메소드. 
		void CancleMatch(const int sessionId)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			// 요청받은 세션을 찾아 삭제한다.
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

			// TODO :: 상대 전적 볼 수 있도록.
			matchNotify1._enemyWins = 0;
			matchNotify1._enemyLoses = 0;

			matchNotify1._gameNumber = gameIdx;

			matchNotify2._enemyId = user1->GetId();
			matchNotify2._enemyType = static_cast<int>(user1->GetCharacterType());

			// TODO :: 마찬가지로 상대 전적 볼 수 있도록.
			matchNotify2._enemyWins = 0;
			matchNotify2._enemyLoses = 0;

			matchNotify2._gameNumber = gameIdx;

			// WARN :: Util 클래스의 PushToSendQueue는 _sendQueue를 인자로 받기 때문에 위험할 수도 있음.
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_MatchSuccessNotify, user1->GetSessionIdx(), &matchNotify1);
			Util::PushToSendQueue(_sendQueue, Packet::PacketId::ID_MatchSuccessNotify, user2->GetSessionIdx(), &matchNotify2);
		}

		// 매칭을 기다리는 큐를 가지고 있음.
		// TODO :: 전용 자료구조를 만들어주는 것이 좋을 것 같다.
		// 지금은 시간이 없어서 deque로 대체.
		std::deque<int>       _matchingQueue;
		std::mutex            _mutex;

		ConsoleLogger   *     _logger;
		PacketQueue     *     _sendQueue;
		GameRoomManager *     _gameRoomManager;
		UserManager		*	  _userManager;
	};
}

#pragma once
#include <memory>
#include <unordered_map>
#include <list>
#include <functional>

#include "../../include/json/json.h"

#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/ErrorCode.h"
#include "../../Common/Packet.h"

#include "../../Network/Network/NetworkMessenger.h"
#include "../../Network/Network/PacketInfo.h"

#include "GameRoomManager.h"
#include "UserManager.h"
#include "MatchMaker.h"
#include "User.h"

namespace FPLogic
{
	using PacketQueue   = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using LogType       = FPCommon::LogType;
	using ErrorCode     = FPCommon::ErrorCode;

	using NetworkMessenger = FPNetwork::NetworkMessenger;
	using PacketInfo       = FPNetwork::PacketInfo;

	class PacketProcess
	{
		// ��Ŷ�� ó���ϴ� �Լ� ���� ����.
		using PacketFunction     = std::function<void(std::shared_ptr<PacketInfo>)>;
		// ��Ŷ ó�� �Լ� ����Ʈ ����.
		using PacketFunctionList = std::list<PacketFunction>;
		using UserState = User::UserState;

	public :

		PacketProcess() {}
		~PacketProcess() {}

		void Init(ConsoleLogger * logger, NetworkMessenger * network, MatchMaker * matchMaker, UserManager * userManager, PacketQueue * recvQueue, PacketQueue * sendQueue, GameRoomManager * gameRoomManager);

	private :

		// ����� �Լ����� �ʱ⿡ ������ִ� �޼ҵ�.
		void RegistPacketFunctions();

		void process();

		// ��Ŷ ó�� �Լ� ��� �޼ҵ�.
		void subscribe(short interestedPacketId, PacketFunction registFunction);

		// ��ϵ� �޼ҵ���� ȣ�����ִ� �޼ҵ�.
		void broadCast(std::shared_ptr<PacketInfo> recvPacket);

		// ��Ŷ ���� �Լ���.
		void LoginReq         (std::shared_ptr<PacketInfo> packet);
		void FastMatchReq     (std::shared_ptr<PacketInfo> packet);
		void MatchCancelReq   (std::shared_ptr<PacketInfo> packet);
		void MatchSuccessAck  (std::shared_ptr<PacketInfo> packet);
		void GameStartAck     (std::shared_ptr<PacketInfo> packet);
		void TurnStartAck     (std::shared_ptr<PacketInfo> packet);
		void EnemyTurnStartAck(std::shared_ptr<PacketInfo> packet);
		void MoveNotify       (std::shared_ptr<PacketInfo> packet);
		void EnemyMoveAck     (std::shared_ptr<PacketInfo> packet);
		void FireNotify       (std::shared_ptr<PacketInfo> packet);
		void EnemyFireAck     (std::shared_ptr<PacketInfo> packet);
		void GameSetAck       (std::shared_ptr<PacketInfo> packet);
		void CloseReq         (std::shared_ptr<PacketInfo> packet);
		
		// ��Ŷ ó���� ������ �޼ҵ�.
		void PacketUnpack(std::shared_ptr<PacketInfo> packet, Packet::IJsonSerializable * outSturct);
		void PushToSendQueue(Packet::PacketId packetId, const int sessionIdx, Packet::IJsonSerializable * packetToSend);

	private :

		ConsoleLogger    * _logger          = nullptr;
		UserManager      * _userManager     = nullptr;
		PacketQueue      * _recvQueue       = nullptr;
		PacketQueue      * _sendQueue       = nullptr;
		NetworkMessenger * _network         = nullptr;
		MatchMaker       * _matchMaker      = nullptr;
		GameRoomManager  * _gameRoomManager = nullptr;

		// ��Ŷ ��ȣ�� ��ϵ� ��Ŷ ó�� �Լ� ����Ʈ�� �����ϴ� �ڷᱸ��.
		std::unordered_map<short, PacketFunctionList> _packetFunctionMap;
	};
}
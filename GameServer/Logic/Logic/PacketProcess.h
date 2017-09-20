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
		// 패킷을 처리하는 함수 형태 정의.
		using PacketFunction     = std::function<void(std::shared_ptr<PacketInfo>)>;
		// 패킷 처리 함수 리스트 정의.
		using PacketFunctionList = std::list<PacketFunction>;
		using UserState = User::UserState;

	public :

		PacketProcess() {}
		~PacketProcess() {}

		void Init(ConsoleLogger * logger, NetworkMessenger * network, MatchMaker * matchMaker, UserManager * userManager, PacketQueue * recvQueue, PacketQueue * sendQueue, GameRoomManager * gameRoomManager);

	private :

		// 사용할 함수들을 초기에 등록해주는 메소드.
		void RegistPacketFunctions();

		void process();

		// 패킷 처리 함수 등록 메소드.
		void subscribe(short interestedPacketId, PacketFunction registFunction);

		// 등록된 메소드들을 호출해주는 메소드.
		void broadCast(std::shared_ptr<PacketInfo> recvPacket);

		// 패킷 관련 함수들.
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
		
		// 패킷 처리를 도와줄 메소드.
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

		// 패킷 번호와 등록된 패킷 처리 함수 리스트를 저장하는 자료구조.
		std::unordered_map<short, PacketFunctionList> _packetFunctionMap;
	};
}
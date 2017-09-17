#include "PacketProcess.h"

#include <thread>
#include <memory>
#include <chrono>
#include <functional>

#include "../../Common/Packet.h"

#include "MatchMaker.h"
#include "UserManager.h"
#include "GameRoomManager.h"

namespace FPLogic
{
	void PacketProcess::Init(
		ConsoleLogger    * logger,
		NetworkMessenger * network,
		MatchMaker		 * matchMaker,
		UserManager      * userManager,
		PacketQueue      * recvQueue,
		PacketQueue      * sendQueue,
		GameRoomManager  * gameRoomManager)
	{
		_logger      = logger;
		_network     = network;
		_matchMaker  = matchMaker;
		_userManager = userManager;
		_recvQueue   = recvQueue;
		_sendQueue   = sendQueue;
		_gameRoomManager = gameRoomManager;

		// 패킷 관련 함수 등록하기.
		RegistPacketFunctions();

		// 업데이트 쓰레드 돌려주기.
		auto processThread = std::thread(std::bind(&PacketProcess::process, this));
		processThread.detach();
	}

	void PacketProcess::RegistPacketFunctions()
	{
		using namespace Packet;

		subscribe(static_cast<short>(PacketId::ID_LoginReq         ), std::bind(&PacketProcess::LoginReq         , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_FastMatchReq     ), std::bind(&PacketProcess::FastMatchReq     , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_MatchCancelReq   ), std::bind(&PacketProcess::MatchCancelReq   , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_MatchSuccessAck  ), std::bind(&PacketProcess::MatchSuccessAck  , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_GameStartAck     ), std::bind(&PacketProcess::GameStartAck     , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_TurnStartAck     ), std::bind(&PacketProcess::TurnStartAck     , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_EnemyTurnStartAck), std::bind(&PacketProcess::EnemyTurnStartAck, this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_MoveNotify       ), std::bind(&PacketProcess::MoveNotify       , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_EnemyMoveAck     ), std::bind(&PacketProcess::EnemyMoveAck     , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_FireNotify       ), std::bind(&PacketProcess::FireNotify       , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_EnemyFireAck     ), std::bind(&PacketProcess::EnemyFireAck     , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_GameSetAck       ), std::bind(&PacketProcess::GameSetAck       , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_CloseReq         ), std::bind(&PacketProcess::CloseReq         , this, std::placeholders::_1));
	}

	void PacketProcess::process()
	{
		while (true)
		{
			auto packet = _recvQueue->Peek();

			// 받은 패킷이 없다면
			if (packet == nullptr)
			{
				// 다른 쓰레드에게 양보한다.
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Packet Process Id(%d)", __FUNCTION__, packet->_packetId);

			// 받은 패킷이 있다면 브로드 캐스트.
			broadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::subscribe(short interestedPacketId, PacketFunction registFunction)
	{
		// 등록하려는 패킷 아이디로 이미 다른 리스트가 등록되어있는지 확인.
		auto findListResult = _packetFunctionMap.find(interestedPacketId);

		// 등록이 안되어있다면 새로 리스트 생성.
		if (findListResult == _packetFunctionMap.end())
		{
			PacketFunctionList newList;
			_packetFunctionMap.emplace(interestedPacketId, newList);
		}
			
		// 함수 등록.
		_packetFunctionMap.find(interestedPacketId)->second.emplace_back(std::move(registFunction));
	}

	void PacketProcess::broadCast(std::shared_ptr<PacketInfo> recvPacket)
	{
		// 들어온 패킷에 해당하는 함수 목록을 찾는다.
		auto subscribedFunctions = _packetFunctionMap.find(recvPacket->_packetId);

		// 대응되는 함수가 없다면 워닝을 띄운다.
		if (subscribedFunctions == _packetFunctionMap.end())
		{
			_logger->Write(LogType::LOG_WARN, "%s | Unregisted packet id arrived Id(%d)", __FUNCTION__, recvPacket->_packetId);
			return;
		}

		// 대응되는 함수 목록들을 쭉 실행시킨다.
		for (auto& function : subscribedFunctions->second)
		{
			function(recvPacket);
		}
	}

	// 패킷 데이터 처리를 도와주는 메소드들.
	void PacketProcess::PacketUnpack(std::shared_ptr<PacketInfo> packet, Packet::IJsonSerializable * outSturct)
	{
		auto rawJson = std::string(packet->_body);
		Json::Value root;
		Json::Reader reader;

		reader.parse(rawJson.c_str(), root);

		outSturct->Deserialize(root);
	}

	void PacketProcess::PushToSendQueue(Packet::PacketId packetId, const int sessionIdx, Packet::IJsonSerializable * packetToSend)
	{
		auto jsonBody = std::string();
		Packet::CJsonSerializer::Serialize(packetToSend, jsonBody);
		auto bodySize = static_cast<int>(strlen(jsonBody.c_str())) + 1;
		auto sendPacket = std::make_shared<PacketInfo>();
		sendPacket->SetPacketInfo(packetId, sessionIdx, bodySize, jsonBody.c_str());

		_sendQueue->Push(sendPacket);
	}
}

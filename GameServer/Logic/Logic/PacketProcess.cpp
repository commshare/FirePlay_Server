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

		// ��Ŷ ���� �Լ� ����ϱ�.
		RegistPacketFunctions();

		// ������Ʈ ������ �����ֱ�.
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

			// ���� ��Ŷ�� ���ٸ�
			if (packet == nullptr)
			{
				// �ٸ� �����忡�� �纸�Ѵ�.
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Packet Process Id(%d)", __FUNCTION__, packet->_packetId);

			// ���� ��Ŷ�� �ִٸ� ��ε� ĳ��Ʈ.
			broadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::subscribe(short interestedPacketId, PacketFunction registFunction)
	{
		// ����Ϸ��� ��Ŷ ���̵�� �̹� �ٸ� ����Ʈ�� ��ϵǾ��ִ��� Ȯ��.
		auto findListResult = _packetFunctionMap.find(interestedPacketId);

		// ����� �ȵǾ��ִٸ� ���� ����Ʈ ����.
		if (findListResult == _packetFunctionMap.end())
		{
			PacketFunctionList newList;
			_packetFunctionMap.emplace(interestedPacketId, newList);
		}
			
		// �Լ� ���.
		_packetFunctionMap.find(interestedPacketId)->second.emplace_back(std::move(registFunction));
	}

	void PacketProcess::broadCast(std::shared_ptr<PacketInfo> recvPacket)
	{
		// ���� ��Ŷ�� �ش��ϴ� �Լ� ����� ã�´�.
		auto subscribedFunctions = _packetFunctionMap.find(recvPacket->_packetId);

		// �����Ǵ� �Լ��� ���ٸ� ������ ����.
		if (subscribedFunctions == _packetFunctionMap.end())
		{
			_logger->Write(LogType::LOG_WARN, "%s | Unregisted packet id arrived Id(%d)", __FUNCTION__, recvPacket->_packetId);
			return;
		}

		// �����Ǵ� �Լ� ��ϵ��� �� �����Ų��.
		for (auto& function : subscribedFunctions->second)
		{
			function(recvPacket);
		}
	}

	// ��Ŷ ������ ó���� �����ִ� �޼ҵ��.
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

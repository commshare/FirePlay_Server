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

		//注册与数据包相关的功能。
		RegistPacketFunctions();

		//返回更新线程。
		auto processThread = std::thread(std::bind(&PacketProcess::process, this));
		/*
		detach是用来和线程对象分离的，这样线程可以独立地执行，不过这样由于没有thread对象指向该线程而失去了对它的控制，
		当对象析构时线程会继续在后台执行，但是当主程序退出时并不能保证线程能执行完。
		如果没有良好的控制机制或者这种后台线程比较重要，最好不用detach而应该使用join。
		*/
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
		subscribe(static_cast<short>(PacketId::ID_TurnEndNotify    ), std::bind(&PacketProcess::TurnEndNofity    , this, std::placeholders::_1));
		subscribe(static_cast<short>(PacketId::ID_DamageOccur	   ), std::bind(&PacketProcess::DamageOccur		 , this, std::placeholders::_1));
	}

	void PacketProcess::process()
	{
		while (true)
		{
			auto packet = _recvQueue->Peek();

			//如果没有收到的数据包
			if (packet == nullptr)
			{
				// 放弃另一个帖子。
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Packet Process Id(%d)", __FUNCTION__, packet->_packetId);

			//如果收到数据包，则广播。
			broadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::subscribe(short interestedPacketId, PacketFunction registFunction)
	{
		//检查是否已在注册的数据包ID中注册了另一个列表。
		auto findListResult = _packetFunctionMap.find(interestedPacketId);

		//如果未注册，请创建新列表。
		if (findListResult == _packetFunctionMap.end())
		{
			PacketFunctionList newList;
			_packetFunctionMap.emplace(interestedPacketId, newList);
		}
			
		//注册功能。
		_packetFunctionMap.find(interestedPacketId)->second.emplace_back(std::move(registFunction));
	}

	void PacketProcess::broadCast(std::shared_ptr<PacketInfo> recvPacket)
	{
		//查找与传入数据包对应的功能列表。
		auto subscribedFunctions = _packetFunctionMap.find(recvPacket->_packetId);

		//如果没有相应的功能，则引发警告。
		if (subscribedFunctions == _packetFunctionMap.end())
		{
			_logger->Write(LogType::LOG_WARN, "%s | Unregisted packet id arrived Id(%d)", __FUNCTION__, recvPacket->_packetId);
			return;
		}

		//始终执行相应的功能列表。
		for (auto& function : subscribedFunctions->second)
		{
			function(recvPacket);
		}
	}

	//帮助处理分组数据的方法。
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
		//使用strcpy需要多一个+1
		auto bodySize = static_cast<int>(strlen(jsonBody.c_str())) + 1;
		auto sendPacket = std::make_shared<PacketInfo>();
		sendPacket->SetPacketInfo(packetId, sessionIdx, bodySize, jsonBody.c_str());

		_sendQueue->Push(sendPacket);
	}
}

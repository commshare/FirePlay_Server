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

		//ע�������ݰ���صĹ��ܡ�
		RegistPacketFunctions();

		//���ظ����̡߳�
		auto processThread = std::thread(std::bind(&PacketProcess::process, this));
		/*
		detach���������̶߳������ģ������߳̿��Զ�����ִ�У�������������û��thread����ָ����̶߳�ʧȥ�˶����Ŀ��ƣ�
		����������ʱ�̻߳�����ں�ִ̨�У����ǵ��������˳�ʱ�����ܱ�֤�߳���ִ���ꡣ
		���û�����õĿ��ƻ��ƻ������ֺ�̨�̱߳Ƚ���Ҫ����ò���detach��Ӧ��ʹ��join��
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

			//���û���յ������ݰ�
			if (packet == nullptr)
			{
				// ������һ�����ӡ�
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Packet Process Id(%d)", __FUNCTION__, packet->_packetId);

			//����յ����ݰ�����㲥��
			broadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::subscribe(short interestedPacketId, PacketFunction registFunction)
	{
		//����Ƿ�����ע������ݰ�ID��ע������һ���б�
		auto findListResult = _packetFunctionMap.find(interestedPacketId);

		//���δע�ᣬ�봴�����б�
		if (findListResult == _packetFunctionMap.end())
		{
			PacketFunctionList newList;
			_packetFunctionMap.emplace(interestedPacketId, newList);
		}
			
		//ע�Ṧ�ܡ�
		_packetFunctionMap.find(interestedPacketId)->second.emplace_back(std::move(registFunction));
	}

	void PacketProcess::broadCast(std::shared_ptr<PacketInfo> recvPacket)
	{
		//�����봫�����ݰ���Ӧ�Ĺ����б�
		auto subscribedFunctions = _packetFunctionMap.find(recvPacket->_packetId);

		//���û����Ӧ�Ĺ��ܣ����������档
		if (subscribedFunctions == _packetFunctionMap.end())
		{
			_logger->Write(LogType::LOG_WARN, "%s | Unregisted packet id arrived Id(%d)", __FUNCTION__, recvPacket->_packetId);
			return;
		}

		//ʼ��ִ����Ӧ�Ĺ����б�
		for (auto& function : subscribedFunctions->second)
		{
			function(recvPacket);
		}
	}

	//��������������ݵķ�����
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
		//ʹ��strcpy��Ҫ��һ��+1
		auto bodySize = static_cast<int>(strlen(jsonBody.c_str())) + 1;
		auto sendPacket = std::make_shared<PacketInfo>();
		sendPacket->SetPacketInfo(packetId, sessionIdx, bodySize, jsonBody.c_str());

		_sendQueue->Push(sendPacket);
	}
}

#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"

#include "../../Network/Network/PacketInfo.h"

namespace FPLogic
{
	void PacketProcess::LoginReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��û ��Ŷ ������ ��´�.
		Json::Value jsonData = packet->_body;
		Packet::LoginReq loginReq;
		loginReq.Deserialize(jsonData);

		// Http�� ����Ͽ� Validation���� Ȯ��.
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// ���� ��Ŷ�� �غ�.
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// ��ȿ�� ��û�̾��ٸ� �ش� ������ UserManager�� �߰�.
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: User�� ������ DB���� �޾ƿͼ� �������־�� ��.

		// �亯 ����.
		auto sendPacket = std::make_shared<PacketInfo>();
		sendPacket->_packetId = Packet::PacketId::ID_LoginRes;
		sendPacket->_sessionIdx = packet->_sessionIdx;

		auto jsonBody = std::string();
		Packet::CJsonSerializer::Serialize(&loginRes, jsonBody);

		sendPacket->_bodySize = static_cast<int>(strlen(jsonBody.c_str())) + 1;
		sendPacket->_body = (char*)jsonBody.c_str();

		_sendQueue->Push(sendPacket);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::MatchSuccessAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::TurnStartAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::EnemyTurnStartAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::MoveNotify(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::EnemyMoveAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::FireNotify(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::EnemyFireAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::GameSetAck(std::shared_ptr<PacketInfo> packet)
	{
	}

	void PacketProcess::CloseReq(std::shared_ptr<PacketInfo> packet)
	{
	}


}
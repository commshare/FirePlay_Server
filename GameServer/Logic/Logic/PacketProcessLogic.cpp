#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"

#include "../../Network/Network/PacketInfo.h"

#include "Define.h"

namespace FPLogic
{
	void PacketProcess::LoginReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��û ��Ŷ ������ ��´�.
		Packet::LoginReq loginReq;
		PacketUnpack(packet, &loginReq);

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
		PushToSendQueue(Packet::PacketId::ID_LoginRes, packet->_sessionIdx, &loginRes);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::FastMatchReq fastMatchReq;
		PacketUnpack(packet, &fastMatchReq);

		// ��û�� ������ ���¸� �������ش�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr)
		{
			// �̻��� ��û�� ���Դٰ� �����ϰ� �����Ѵ�.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Req Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}
		reqUser->JoinMatching(static_cast<CharacterType>(fastMatchReq._type), MatchingType::FastMatch);

		// ��û�� ������ �������� ��ġ ����Ŀ�� �־��ش�.
		_matchMaker->RequestMatch(packet->_sessionIdx);

		// ����� ��ȯ�Ѵ�.
		Packet::FastMatchRes matchRes;
		matchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FastMatchRes, packet->_sessionIdx, &matchRes);
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::MatchCancelReq matchCancelReq;
		PacketUnpack(packet, &matchCancelReq);

		// ��û�� ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr)
		{
			// �̻��� ��û�� ���Դٰ� �����ϰ� �����Ѵ�.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Cancel Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ��û�� ������ ��Īť�� �ִٸ� ��Īť���� ���ش�.
		if (_matchMaker->isUserIsInMatching(packet->_sessionIdx))
		{

		}

		// ����� ��ȯ�Ѵ�.
		Packet::MatchCancelRes cancelRes;
		cancelRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MatchCancelRes, packet->_sessionIdx, &cancelRes);
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
#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
#include "../../Common/Util.h"

#include "../../Network/Network/PacketInfo.h"
#include "../../Network/Network/HttpNetwork.h"

namespace FPLogic
{
	void PacketProcess::LoginReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 요청 패킷 정보를 얻는다.
		Packet::LoginReq loginReq;
		FPCommon::Util::DeserializeFromCharByte(&loginReq, packet);

		// Http와 통신하여 Validation한지 확인.
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// 보낼 패킷을 준비.
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// 유효한 요청이었다면 해당 세션을 UserManager에 추가.
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: User의 전적을 DB에서 받아와서 갱신해주어야 함.

		// 답변 전달.
		auto sendPacket = std::make_shared<PacketInfo>();
		sendPacket->_packetId = Packet::PacketId::ID_LoginRes;
		sendPacket->_sessionIdx = packet->_sessionIdx;
		sendPacket->_bodySize = sizeof(Packet::LoginRes);
		sendPacket->_body = (char*)&loginRes;
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
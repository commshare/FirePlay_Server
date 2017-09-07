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

		// 요청 패킷 정보를 얻는다.
		auto bodyString = std::string(packet->_body);
		Json::Value jsonData;
		std::string err;

		auto a = new Json::CharReaderBuilder();
		auto reader = a->newCharReader();
		reader->parse(packet->_body, packet->_body + packet->_bodySize, &jsonData, &err);

		auto _id = jsonData.get("_id", "").asString();
		Packet::LoginReq req;
		req.Deserialize(jsonData);

		// Http와 통신하여 Validation한지 확인.
		
		// Validation하지 않다면 에러 코드 전달.

		// Validation하다면 해당 세션을 ConnectedUser에 추가.

		// 정상적인 답변 전달.
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
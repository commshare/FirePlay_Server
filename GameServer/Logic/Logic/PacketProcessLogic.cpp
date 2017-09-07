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
		Packet::LoginReq req;
		DeserializeFromCharByte(&req, packet);

		// Http�� ����Ͽ� Validation���� Ȯ��.

		
		// Validation���� �ʴٸ� ���� �ڵ� ����.

		// Validation�ϴٸ� �ش� ������ ConnectedUser�� �߰�.

		// �������� �亯 ����.
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

	// Char Byte ���¿��� Deserialize�� �������ִ� �޼ҵ�.
	void PacketProcess::DeserializeFromCharByte(Packet::IJsonSerializable * outResult, std::shared_ptr<PacketInfo> packetInfo) 
	{
		Json::Value jsonValue;
		std::string errorMessage;

		auto readerBuilder = new Json::CharReaderBuilder();
		auto reader = readerBuilder->newCharReader();
		reader->parse(packetInfo->_body, packetInfo->_body + packetInfo->_bodySize, &jsonValue, &errorMessage);

		// TODO :: ���� Ȯ��.

		outResult->Deserialize(jsonValue);
	}
}
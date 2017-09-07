#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"

#include "../../Network/Network/PacketInfo.h"
#include "../../Network/Network/HttpNetwork.h"

namespace FPLogic
{
	void PacketProcess::LoginReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 요청 패킷 정보를 얻는다.
		Packet::LoginReq req;
		DeserializeFromCharByte(&req, packet);

		// Http와 통신하여 Validation한지 확인.
		Json::Value tokenValidation;
		tokenValidation["Token"] = req._token;
		tokenValidation["UserId"] = req._id;

		// WARN :: 내가 HttpNetwork를 Include해서 저 enum을 가져다 쓰는게 맞는지..?
		// TODO :: 어차피 DB 서버랑 통신하는 경로가 얼마 안될텐데 API를 따로 뚫어주는게 더 좋을 수도.
		auto userValidationString = _network->GetHttp()->PostRequestToDBServer(tokenValidation.toStyledString(), FPNetwork::ApiEnum::TokenValidation);
		
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

	// Char Byte 형태에서 Deserialize를 진행해주는 메소드.
	void PacketProcess::DeserializeFromCharByte(Packet::IJsonSerializable * outResult, std::shared_ptr<PacketInfo> packetInfo) 
	{
		Json::Value jsonValue;
		std::string errorMessage;

		auto readerBuilder = new Json::CharReaderBuilder();
		auto reader = readerBuilder->newCharReader();
		reader->parse(packetInfo->_body, packetInfo->_body + packetInfo->_bodySize, &jsonValue, &errorMessage);

		// TODO :: 에러 확인.

		outResult->Deserialize(jsonValue);
	}
}
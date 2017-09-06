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
		Json::Value jsonData = packet->_body;
		Packet::LoginReq req;
		req.Deserialize(jsonData);

		// Http와 통신하여 Validation한지 확인.
		
		// Validation하지 않다면 에러 코드 전달.

		// Validation하다면 해당 세션을 ConnectedUser에 추가.

		// 정상적인 답변 전달.
	}


}
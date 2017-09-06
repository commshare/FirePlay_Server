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
		Packet::LoginReq req;
		req.Deserialize(jsonData);

		// Http�� ����Ͽ� Validation���� Ȯ��.
		
		// Validation���� �ʴٸ� ���� �ڵ� ����.

		// Validation�ϴٸ� �ش� ������ ConnectedUser�� �߰�.

		// �������� �亯 ����.
	}


}
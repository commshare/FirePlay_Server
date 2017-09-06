#include "HttpNetwork.h"
#include <fstream>
#include <memory>
#include <string>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/json.hpp"
#include "../../Common/ErrorCode.h"

#include "PacketInfo.h"

namespace FPNetwork
{
	using json = nlohmann::json;

	ErrorCode HttpNetwork::Init(ConsoleLogger * logger, PacketQueue * recvQueue)
	{
		_logger = logger;
		_recvQueue = recvQueue;

		// TODO :: DB ���� �ּҸ� config���� ��������.
		LoadHttpConfig();
	}

	ErrorCode HttpNetwork::LoadHttpConfig()
	{
		std::ifstream configFile("HttpConfig.json");
		if (configFile.fail())
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Config File Load Failed", __FUNCTION__);
			return ErrorCode::FailConfigLoad;
		}

		json configJson;
		configFile >> configJson;

	}

	void HttpNetwork::OnBegin(const happyhttp::Response * r, void * userdata)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Begin HTTP POST Status(%d : %s)", __FUNCTION__, r->getstatus(), r->getreason());
	}

	void HttpNetwork::OnData(const happyhttp::Response * r, void * userdata, const unsigned char * data, int n)
	{
		auto recvPacket = std::make_shared<PacketInfo>();
	}

	void HttpNetwork::OnComplete(const happyhttp::Response * r, void * userdata)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Complete HTTP POST Status(%d : %s)", __FUNCTION__, r->getstatus(), r->getreason());
	}


}

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

		// TODO :: DB 서버 주소를 config에서 가져오기.
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

		_config = std::make_unique<HttpConfig>();

		_config->_dbServerUrl = configJson["_dbServerUrl"].get<std::string>();
		_config->_dbServerPort = std::stoi(configJson["_dbServerPort"].get<std::string>());

		std::string api = "api";
		for (auto i = 1; i < static_cast<int>(HttpConfig::ApiEnum::ApiMaxNum); ++i)
		{
			auto currentApi = api + std::to_string(i);
			_config->_dbServerApi[i] = configJson[currentApi].get<std::string>();
		}
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

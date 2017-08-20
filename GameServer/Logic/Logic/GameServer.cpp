#include "GameServer.h"

#include <fstream>
#include <memory>
#include <string>

#include "../../Common/json.hpp"
#include "../../Common/Define.h"
#include "../../Common/ConsoleLogger.h"

#include "../../Network/Network/NetworkMessenger.h"

namespace FPLogic
{
	using json = nlohmann::json;
	using LogType = FPCommon::LogType;

	ErrorCode GameServer::Init()
	{
		// ConsoleLogger 생성.
		_logger = std::make_unique<ConsoleLogger>();

		// 서버 정보 로드.
		if (loadConfig() != ErrorCode::None)
		{
			return ErrorCode::FailConfigLoad;
		}


		return ErrorCode::None;
	}

	void GameServer::Run()
	{
	}

	void GameServer::Stop()
	{
	}

	ErrorCode GameServer::loadConfig()
	{
		std::ifstream configFile("C:\\ServerConfig.json");
		if (configFile.fail())
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Config File Load Failed", __FUNCTION__);
			return ErrorCode::FailConfigLoad;
		}

		json configJson;
		configFile >> configJson;

		_config = std::make_unique<ServerConfig>();

		_config->_port = std::stoi(configJson["_port"].get<std::string>());
		_config->_backlog = std::stoi(configJson["_backlog"].get<std::string>());

		_config->_maxClientCount = std::stoi(configJson["_maxClientCount"].get<std::string>());
		_config->_extraClientCount = std::stoi(configJson["_extraClientCount"].get<std::string>());

		_config->_maxClientRecvSize = std::stoi(configJson["_maxClientRecvSize"].get<std::string>());
		_config->_maxClientSendSize = std::stoi(configJson["_maxClientSendSize"].get<std::string>());

		return ErrorCode::None;
	}

	void GameServer::Release()
	{
		if (_network != nullptr)
		{
			_network->Stop();
		}
	}
}
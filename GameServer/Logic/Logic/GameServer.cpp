#include "GameServer.h"

#include <fstream>
#include <memory>

#include "../../Common/json.hpp"
#include "../../Common/Define.h"

#include "../../Network/Network/NetworkMessenger.h"

namespace FPLogic
{
	using json = nlohmann::json;

	ErrorCode GameServer::Init()
	{
		// 서버 정보 로드.
		loadConfig();


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
		std::ifstream configFile("ServerConfig.json");
		json configJson;
		configFile >> configJson;

		_config = std::make_unique<ServerConfig>();

		_config->_port = configJson["_port"].get<unsigned short>();
		_config->_backlog = configJson["_backlog"].get<int>();

		_config->_maxClientCount = configJson["_maxClientCount"].get<int>();
		_config->_extraClientCount = configJson["_extraClientCount"].get<int>();

		_config->_maxClientRecvSize = configJson["_maxClientRecvSize"].get<short>();
		_config->_maxClientSendSize = configJson["_maxClientSendSize"].get<short>();
	}

	void GameServer::Release()
	{
		if (_network != nullptr)
		{
			_network->Stop();
		}
	}
}
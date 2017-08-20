#include "GameServer.h"

#include <fstream>

#include  "../../Common/json.hpp"

#include "../../Network/Network/NetworkMessenger.h"

namespace FPLogic
{
	using json = nlohmann::json;

	ErrorCode GameServer::Init()
	{
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
	}

	void GameServer::Release()
	{
		if (_network != nullptr)
		{
			_network->Stop();
		}
	}
}
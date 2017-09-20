#include "GameServer.h"

#include <fstream>
#include <memory>
#include <string>

#include "../../Common/json.hpp"
#include "../../Common/Define.h"
#include "../../Common/ConsoleLogger.h"

#include "../../Network/Network/NetworkMessenger.h"

#include "PacketProcess.h"
#include "UserManager.h"
#include "MatchMaker.h"

namespace FPLogic
{
	using json = nlohmann::json;
	using LogType = FPCommon::LogType;

	ErrorCode GameServer::Init()
	{
		// ConsoleLogger ����.
		_logger = std::make_unique<ConsoleLogger>();

		// ���� ���� �ε�.
		if (loadConfig() != ErrorCode::None)
		{
			return ErrorCode::FailConfigLoad;
		}

		// ��Ŷ ť ����.
		_recvQueue = std::make_unique<PacketQueue>();
		_sendQueue = std::make_unique<PacketQueue>();

		// ��Ʈ��ũ �޽��� ����.
		_network = std::make_unique<NetworkMessenger>();
		_network->Init(_logger.get(), _config.get(), _recvQueue.get(), _sendQueue.get());
		
		// ���� ������ Ŭ���� ����.
		_userManager = std::make_unique<UserManager>();
		_userManager->Init(_config.get()->_maxClientCount);

		// ���� �� ������ Ŭ���� ����.
		_gameRoomManager = std::make_unique<GameRoomManager>();
		_gameRoomManager->Init(_logger.get(), _sendQueue.get(), _userManager.get());
		
		// ��ġ ������ Ŭ���� ����.
		_matchMaker = std::make_unique<MatchMaker>();
		_matchMaker->Init(_logger.get(), _sendQueue.get(), _gameRoomManager.get(), _userManager.get());

		// ��Ŷ ó�� Ŭ���� ����
		_packetProcess = std::make_unique<PacketProcess>();
		_packetProcess->Init(_logger.get(), _network.get(), _matchMaker.get(), _userManager.get(), _recvQueue.get(), _sendQueue.get(), _gameRoomManager.get());

		_isServerInitialized = true;
		return ErrorCode::None;
	}

	void GameServer::Stop()
	{
		Release();
	}

	void GameServer::Run()
	{
		while (true)
		{
			if (_isServerInitialized)
			{
				_matchMaker->CheckMatchMaked();
				_gameRoomManager->UpdateRooms();
			}
		}
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
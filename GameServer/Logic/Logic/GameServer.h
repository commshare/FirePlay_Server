#pragma once

#include <memory>

#include "../../Common/ErrorCode.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/PacketQueue.h"

#include "../../Network/Network/NetworkMessenger.h"
#include "../../Network/Network/SessionInfo.h"

namespace FPLogic
{
	using ErrorCode     = FPCommon::ErrorCode;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using ServerConfig  = FPCommon::ServerConfig;
	using PacketQueue   = FPCommon::PacketQueue;

	using NetworkMessenger = FPNetwork::NetworkMessenger;

	class GameServer
	{
	public :

		GameServer() {}
		~GameServer() {}

		ErrorCode Init();

		void Run();

		void Stop();

	private :

		ErrorCode loadConfig();

		void Release();

	private :

		std::unique_ptr<ConsoleLogger> _logger;
		std::unique_ptr<NetworkMessenger> _network;
		std::unique_ptr<ServerConfig> _config;
		
		std::unique_ptr<PacketQueue> _recvQueue;
		std::unique_ptr<PacketQueue> _sendQueue;

	};
}
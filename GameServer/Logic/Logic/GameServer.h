#pragma once

#include <memory>

#include "../../Common/ErrorCode.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/PacketQueue.h"

#include "../../Network/Network/NetworkMessenger.h"
#include "../../Network/Network/SessionInfo.h"

#include "PacketProcess.h"
#include "UserManager.h"
#include "MatchMaker.h"

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

		void Stop();
		void Run();

	private :

		ErrorCode loadConfig();

		void Release();

	private :

		std::unique_ptr<ConsoleLogger>    _logger;
		std::unique_ptr<NetworkMessenger> _network;
		std::unique_ptr<ServerConfig>     _config;
		
		std::unique_ptr<PacketQueue>      _recvQueue;
		std::unique_ptr<PacketQueue>      _sendQueue;

		std::unique_ptr<PacketProcess>    _packetProcess;
		std::unique_ptr<UserManager>      _userManager;
		std::unique_ptr<MatchMaker>		  _matchMaker;

		bool _isServerInitialized = false;
	};
}
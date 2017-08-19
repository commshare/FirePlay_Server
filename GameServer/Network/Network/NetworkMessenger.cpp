#include "NetworkMessenger.h"

#include <memory>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/ObjectPool.h"

namespace FPNetwork
{
	bool NetworkMessenger::init(
		ConsoleLogger * logger,
		ServerConfig * serverConfig,
		PacketQueue * recvQueue,
		PacketQueue * sendQueue)
	{
		if (logger       == nullptr ||
			serverConfig == nullptr ||
			recvQueue    == nullptr ||
			sendQueue    == nullptr)
		{
			return false;
		}

		_logger = logger;
		_recvQueue = recvQueue;
		_sendQueue = sendQueue;
		memcpy(&_serverConfig, serverConfig, sizeof(ServerConfig));

		// 클라이언트 세션 풀 초기화.
		_sessionPool.Init(_serverConfig._maxClientCount);
		for (int i = 0; i < _serverConfig._maxClientCount; ++i)
		{
			_sessionPool[i]._tag = i;
			_sessionPool[i]._recvBuffer = new char[_serverConfig._maxClientRecvSize];
		}

		_logger->Write(LogType::LOG_INFO, "IOCPNetwork Create :: Port(%d), Backlog(%d)", _serverConfig._port, _serverConfig._backlog);

		return true;
	}

}
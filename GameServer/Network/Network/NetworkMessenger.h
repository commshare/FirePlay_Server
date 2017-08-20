#pragma once

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <vector>

#include "../../Common/PacketQueue.h"
#include "../../Common/ObjectPool.h"

#include "SessionInfo.h"

namespace FPCommon
{
	enum class LogType;
	class ServerConfig;
	class ConsoleLogger;
}

namespace FPNetwork
{
	using PacketQueue = FPCommon::PacketQueue;
	using PacketInfo = FPCommon::PacketInfo;
	using LogType = FPCommon::LogType;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using ServerConfig = FPCommon::ServerConfig;

	class NetworkMessenger 
	{
	public :

		NetworkMessenger() {}
		~NetworkMessenger() {}

		void Stop();

		void ForcingClose(const int sessionIdx);

		// Getter, Setter
		HANDLE GetIocpHandler() const { return _iocpHandle; }
		int GetSessionPoolSize() { return _sessionPool.GetSize(); }

	private :

		bool init(
			ConsoleLogger * logger,
			ServerConfig * serverConfig,
			PacketQueue * recvQueue,
			PacketQueue * sendQueue);

		void closeSession(const SessionCloseCase closeCase, const SOCKET socket, const int sessionIdx);
		void addToPacketQueue(const int sessionIdx, const short pktId, const short bodySize, char * body);

		void workerThreadFunc();
		void listenThreadFunc();
		void sendThreadFunc();

	private :

		SOCKET _serverSocket = INVALID_SOCKET;
		HANDLE _iocpHandle = INVALID_HANDLE_VALUE;
		ServerConfig _serverConfig;

		using SessionPool = FPCommon::ObjectPool<SessionInfo>;
		SessionPool _sessionPool;
		ConsoleLogger * _logger = nullptr;
		PacketQueue * _recvQueue = nullptr;
		PacketQueue * _sendQueue = nullptr;

	// Factory Patten
	public :

		class Factory
		{
		public :

			NetworkMessenger * Create(
				ConsoleLogger * logger,
				ServerConfig * serverConfig,
				PacketQueue * recvQueue,
				PacketQueue * sendQueue)
			{
				auto product = new NetworkMessenger();
				if (product == nullptr)
				{
					return nullptr;
				}

				auto initResult = product->init(logger, serverConfig, recvQueue, sendQueue);
				if (initResult == false)
				{
					return nullptr;
				}
				return product;
			}
		};

	};

}
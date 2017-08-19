#pragma once

#include "../../Common/ConsoleLogger.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/Define.h"

namespace FPNetwork
{
	using PacketQueue = FPCommon::PacketQueue;
	using PacketInfo = FPCommon::PacketInfo;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using ServerConfig = FPCommon::ServerConfig;

	class NetworkMessenger 
	{
	public :

		NetworkMessenger() {}
		~NetworkMessenger() {}

	private :

		virtual bool init(
			ConsoleLogger * logger,
			ServerConfig * serverConfig,
			PacketQueue * recvQueue,
			PacketQueue * sendQueue);


	private :

		ConsoleLogger * _logger;
		PacketQueue * _recvQueue;
		PacketQueue * _sendQueue;

	// Factory Patten
	public :

		static class Factory
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
#pragma once
#include <memory>

#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"

#include "UserManager.h"

namespace FPLogic
{
	using PacketQueue = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;

	class PacketProcess
	{
	public :

		PacketProcess() {}
		~PacketProcess() {}

		void Init(ConsoleLogger * logger, UserManager * userManager, PacketQueue * recvQueue, PacketQueue * sendQueue);

	private :

	private :

		ConsoleLogger * _logger = nullptr;
		UserManager * _userManager = nullptr;
		PacketQueue * _recvQueue = nullptr;
		PacketQueue * _sendQueue = nullptr;
	};
}
#include "PacketProcess.h"

namespace FPLogic
{
	void PacketProcess::Init(
		ConsoleLogger * logger,
		UserManager * userManager,
		PacketQueue * recvQueue,
		PacketQueue * sendQueue)
	{
		_logger = logger;
		_userManager = userManager;
		_recvQueue = recvQueue;
		_sendQueue = sendQueue;
	}
}

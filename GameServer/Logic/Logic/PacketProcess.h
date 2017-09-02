#pragma once
#include <memory>
#include <unordered_map>
#include <list>
#include <functional>

#include "../../Common/PacketQueue.h"
#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"

#include "UserManager.h"

namespace FPLogic
{
	using PacketInfo = FPCommon::PacketInfo;
	using PacketQueue = FPCommon::PacketQueue;
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using LogType = FPCommon::LogType;

	class PacketProcess
	{
		// ��Ŷ�� ó���ϴ� �Լ� ���� ����.
		using PacketFunction = std::function<void(std::shared_ptr<PacketInfo>)>;
		// ��Ŷ ó�� �Լ� ����Ʈ ����.
		using PacketFunctionList = std::list<PacketFunction>;

	public :

		PacketProcess() {}
		~PacketProcess() {}

		void Init(ConsoleLogger * logger, UserManager * userManager, PacketQueue * recvQueue, PacketQueue * sendQueue);

	private :

		void process();

		// ��Ŷ ó�� �Լ� ��� �޼ҵ�.
		void subscribe(short interestedPacketId, PacketFunction registFunction);

		// ��ϵ� �޼ҵ���� ȣ�����ִ� �޼ҵ�.
		void broadCast(std::shared_ptr<PacketInfo> recvPacket);

	private :

		ConsoleLogger * _logger = nullptr;
		UserManager * _userManager = nullptr;
		PacketQueue * _recvQueue = nullptr;
		PacketQueue * _sendQueue = nullptr;

		// ��Ŷ ��ȣ�� ��ϵ� ��Ŷ ó�� �Լ� ����Ʈ�� �����ϴ� �ڷᱸ��.
		std::unordered_map<short, PacketFunctionList> _packetFunctionMap;
	};
}
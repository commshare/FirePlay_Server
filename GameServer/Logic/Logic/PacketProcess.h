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
		// 패킷을 처리하는 함수 형태 정의.
		using PacketFunction = std::function<void(std::shared_ptr<PacketInfo>)>;
		// 패킷 처리 함수 리스트 정의.
		using PacketFunctionList = std::list<PacketFunction>;

	public :

		PacketProcess() {}
		~PacketProcess() {}

		void Init(ConsoleLogger * logger, UserManager * userManager, PacketQueue * recvQueue, PacketQueue * sendQueue);

	private :

		void process();

		// 패킷 처리 함수 등록 메소드.
		void subscribe(short interestedPacketId, PacketFunction registFunction);

		// 등록된 메소드들을 호출해주는 메소드.
		void broadCast(std::shared_ptr<PacketInfo> recvPacket);

	private :

		ConsoleLogger * _logger = nullptr;
		UserManager * _userManager = nullptr;
		PacketQueue * _recvQueue = nullptr;
		PacketQueue * _sendQueue = nullptr;

		// 패킷 번호와 등록된 패킷 처리 함수 리스트를 저장하는 자료구조.
		std::unordered_map<short, PacketFunctionList> _packetFunctionMap;
	};
}
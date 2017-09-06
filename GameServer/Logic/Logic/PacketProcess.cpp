#include "PacketProcess.h"

#include <thread>
#include <memory>
#include <chrono>
#include <functional>

namespace FPLogic
{
	void PacketProcess::Init(
		ConsoleLogger    * logger,
		NetworkMessenger * network,
		UserManager      * userManager,
		PacketQueue      * recvQueue,
		PacketQueue      * sendQueue)
	{
		_logger      = logger;
		_network     = network;
		_userManager = userManager;
		_recvQueue   = recvQueue;
		_sendQueue   = sendQueue;

		// ��Ŷ ���� �Լ� ����ϱ�.
		RegistPacketFunctions();

		// ������Ʈ ������ �����ֱ�.
		auto processThread = std::thread(std::bind(&PacketProcess::process, this));
		processThread.detach();
	}

	void PacketProcess::RegistPacketFunctions()
	{

	}

	void PacketProcess::process()
	{
		while (true)
		{
			auto packet = _recvQueue->Peek();

			// ���� ��Ŷ�� ���ٸ�
			if (packet == nullptr)
			{
				// �ٸ� �����忡�� �纸�Ѵ�.
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Packet Process Id(%d)", __FUNCTION__, packet->_packetId);

			// ���� ��Ŷ�� �ִٸ� ��ε� ĳ��Ʈ.
			broadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::subscribe(short interestedPacketId, PacketFunction registFunction)
	{
		// ����Ϸ��� ��Ŷ ���̵�� �̹� �ٸ� ����Ʈ�� ��ϵǾ��ִ��� Ȯ��.
		auto findListResult = _packetFunctionMap.find(interestedPacketId);

		// ����� �ȵǾ��ִٸ� ���� ����Ʈ ����.
		if (findListResult == _packetFunctionMap.end())
		{
			PacketFunctionList newList;
			_packetFunctionMap.emplace(interestedPacketId, newList);
		}
			
		// �Լ� ���.
		_packetFunctionMap.find(interestedPacketId)->second.emplace_back(std::move(registFunction));
	}

	void PacketProcess::broadCast(std::shared_ptr<PacketInfo> recvPacket)
	{
		// ���� ��Ŷ�� �ش��ϴ� �Լ� ����� ã�´�.
		auto subscribedFunctions = _packetFunctionMap.find(recvPacket->_packetId);

		// �����Ǵ� �Լ��� ���ٸ� ������ ����.
		if (subscribedFunctions == _packetFunctionMap.end())
		{
			_logger->Write(LogType::LOG_WARN, "%s | Unregisted packet id arrived", __FUNCTION__);
			return;
		}

		// �����Ǵ� �Լ� ��ϵ��� �� �����Ų��.
		for (auto& function : subscribedFunctions->second)
		{
			function(recvPacket);
		}
	}
}

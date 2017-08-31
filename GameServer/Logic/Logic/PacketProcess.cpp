#include "PacketProcess.h"

#include <thread>
#include <memory>
#include <chrono>
#include <functional>

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

		// ������Ʈ ������ �����ֱ�.
	}

	void PacketProcess::Process()
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

			// ���� ��Ŷ�� �ִٸ� ��ε� ĳ��Ʈ.
			BroadCast(packet);
			_recvQueue->Pop();
		}
	}

	void PacketProcess::Subscribe(short interestedPacketId, PacketFunction registFunction)
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

	void PacketProcess::BroadCast(std::shared_ptr<PacketInfo> recvPacket)
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

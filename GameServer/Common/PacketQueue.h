#pragma once
#include <memory>
#include <deque>
#include <mutex>

#include "Define.h"

namespace FirePlayCommon
{
	/*
	 * PacketQueue
	 * ��Ƽ������ ȯ�濡�� ���ư��� ť�� �����ϱ� ���Ͽ� ���� Ŭ����.
	 */
	class PacketQueue
	{
	public:

		PacketQueue() = default;
		~PacketQueue()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_packetDeque.clear();
			_packetDeque.shrink_to_fit();
		}

		std::shared_ptr<PacketInfo> Peek()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_packetDeque.empty())
			{
				return nullptr;
			}
			return _packetDeque.front();
		}

		void Pop()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_packetDeque.pop_front();
		}

		void Push(std::shared_ptr<PacketInfo> recvPacket)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_packetDeque.push_back(recvPacket);
		}

		bool IsEmpty()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _packetDeque.empty();
		};

	private:

		std::deque<std::shared_ptr<PacketInfo>> _packetDeque;
		std::mutex _mutex;
	};

}
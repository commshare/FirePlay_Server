#pragma once
#include "../../Common/Packet.h"

namespace FPNetwork
{
#pragma pack(push, 1)

	struct PacketHeader
	{
		int _id;
		int _bodySize;
	};

	constexpr short packetHeaderSize = sizeof(PacketHeader);

	struct PacketInfo
	{
		PacketInfo() = default;
		void SetPacketInfo(Packet::PacketId id, int sessionIdx, int bodySize, const char * body)
		{
			_packetId = id;
			_sessionIdx = sessionIdx;
			_bodySize = bodySize;
			_body = new char[_bodySize];
			strcpy_s(_body, _bodySize, body);
		}

		~PacketInfo()
		{
			if (_body != nullptr)
			{
				//delete[] _body;
			}
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		char * _body = nullptr;
	};

#pragma pack(pop)
}

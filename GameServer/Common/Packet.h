#pragma once

#include "PacketID.h"
#include "ErrorCode.h"

namespace FPCommon
{
#pragma pack(push, 1)

	struct PacketHeader
	{
		short _id;
		short _bodySize;
	};

	const auto packetHeaderSize = sizeof(PacketHeader);


#pragma pack(pop)
}

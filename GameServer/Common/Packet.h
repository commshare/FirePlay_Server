#pragma once

#include <msgpack.hpp>

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

	struct PacketBase
	{
		short _errorCode = static_cast<short>(ErrorCode::None);
		void SetError(ErrorCode error) { _errorCode = static_cast<short>(error); };
	};

	constexpr int maxUserIdSize = 16;

	struct PacketLoginReq
	{
		char _id[maxUserIdSize] = { 0, };
		long _token = 0L;
	};

	struct PacketLoginRes : PacketBase
	{

	};

#pragma pack(pop)
}

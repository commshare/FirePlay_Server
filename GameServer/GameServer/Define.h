#pragma once

namespace FirePlayCommon
{
	struct PacketInfo
	{
		PacketInfo() = default;
		~PacketInfo()
		{
			if (_body != nullptr)
			{
				delete _body;
				_body = nullptr;
			}
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		char * _body = nullptr;
	};
}
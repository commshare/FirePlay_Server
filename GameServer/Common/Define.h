#pragma once

namespace FPCommon
{
	struct PacketInfo
	{
		PacketInfo() = default;
		~PacketInfo()
		{
			if (_body != nullptr)
			{
				delete[] _body;
				_body = nullptr;
			}
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		char * _body = nullptr;
	};

	class ServerConfig
	{
	public :

		ServerConfig() {};
		~ServerConfig() {};

		unsigned short _port = 0;
		int _backlog = 0;

		int _maxClientCount = 0;
		int _extraClientCount = 0;

		short _maxClientRecvSize = 0;
		short _maxClientSendSize = 0;
	};
}
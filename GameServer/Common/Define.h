#pragma once

namespace FPCommon
{
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
#pragma once
#include <WinSock2.h>

namespace FPNetwork
{
	// IP 문자열 최대 길이
	const int maxIpLen = 32; 
	// 최대 패킷 보디 크기
	const int maxPacketBodySize = 1024; 

	enum class SessionCloseCase : short
	{
		SessionPoolEmpty = 1,
		ForcingClose = 2
	};

	class SessionInfo
	{
	public :

		SessionInfo()
		{
			ZeroMemory(&_socketAddress, sizeof(SOCKADDR_IN));
		}

		~SessionInfo() {}

		bool IsConnected() { return _socket != 0 ? true : false; }

		void Clear()
		{
			_socket = 0;
			_ip[0] = '\0';
			_reServerHostingDataSize = 0;
			_prevReadPosInRecvBuffer = 0;
			_sendSize = 0;
			ZeroMemory(&_socketAddress, sizeof(SOCKADDR_IN));

			delete[] _recvBuffer;
			delete[] _sendBuffer;
			_recvBuffer = nullptr;
			_sendBuffer = nullptr;
		}

		int       _tag = 0;
		SOCKET    _socket = INVALID_SOCKET;
		SOCKADDR_IN _socketAddress;
		char      _ip[maxIpLen] = { 0, };

		char *    _recvBuffer = nullptr;
		int       _reServerHostingDataSize = 0;
		int       _prevReadPosInRecvBuffer = 0;

		char *    _sendBuffer = nullptr;
		int       _sendSize = 0;
	};
}
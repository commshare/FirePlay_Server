#include "NetworkMessenger.h"

#include <memory>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/ObjectPool.h"

#include "SessionInfo.h"
#include "ServerNetworkErrorCode.h"

namespace FPNetwork
{
	void NetworkMessenger::Stop()
	{
	}

	void NetworkMessenger::ForcingClose(const int sessionIdx)
	{
	}

	bool NetworkMessenger::init(
		ConsoleLogger * logger,
		ServerConfig * serverConfig,
		PacketQueue * recvQueue,
		PacketQueue * sendQueue)
	{
		if (logger       == nullptr ||
			serverConfig == nullptr ||
			recvQueue    == nullptr ||
			sendQueue    == nullptr)
		{
			return false;
		}

		_logger = logger;
		_recvQueue = recvQueue;
		_sendQueue = sendQueue;
		memcpy(&_serverConfig, serverConfig, sizeof(ServerConfig));

		// Ŭ���̾�Ʈ ���� Ǯ �ʱ�ȭ.
		_sessionPool.Init(_serverConfig._maxClientCount);
		for (int i = 0; i < _serverConfig._maxClientCount; ++i)
		{
			_sessionPool[i]._tag = i;
			_sessionPool[i]._recvBuffer = new char[_serverConfig._maxClientRecvSize];
		}

		// ���� ���α׷��� ��� ����.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | WSAStartUp Failed", __FUNCTION__);
			return false;
		}

		// IOCP�� �����Ѵ�.
		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		if (_iocpHandle == INVALID_HANDLE_VALUE)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | CreateIoCompletionPort Failed", __FUNCTION__);
			return false;
		}

		// ���� ������ Ȱ��ȭ�Ѵ�.
		_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (_serverSocket == INVALID_SOCKET)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | ServerSocket Failed", __FUNCTION__);
			return false;
		}

		// ���� ���Ͽ� ���� ���� ���ε� �Ѵ�.
		sockaddr_in socketAddr;
		ZeroMemory(&socketAddr, sizeof(socketAddr));
		socketAddr.sin_family = AF_INET;
		socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		socketAddr.sin_port = htons(_serverConfig._port);
		auto result = bind(_serverSocket, (sockaddr*)&socketAddr, sizeof(socketAddr));
		if (result != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket Bind Failed", __FUNCTION__);
			return false;
		}

		_logger->Write(LogType::LOG_INFO, "NetworkMessenger Create :: Port(%d), Backlog(%d)", _serverConfig._port, _serverConfig._backlog);

		// ���õ� ������ listen���ش�.
		result = listen(_serverSocket, _serverConfig._backlog);
		if (result != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket listen Failed", __FUNCTION__);
			return false;
		}
		_logger->Write(LogType::LOG_INFO, "%s | Listen Start. ServerSocketFD(%I64u), BackLog(%d)", __FUNCTION__, _serverSocket, _serverConfig._backlog);

		// Listen �����带 Ȱ��ȭ �Ѵ�.

		// �ý��� ������ �˾ƿ´�.

		// �ھ� ���� �� �� ��ŭ worker �����带 Ȱ��ȭ�Ѵ�.

		// send �����带 Ȱ��ȭ �Ѵ�. 

		return true;
	}

	// ������ ������ �ݾ��ִ� �Լ�.
	void NetworkMessenger::closeSession(const SessionCloseCase closeCase, const SOCKET socket, const int sessionIdx)
	{
		// ���� Ǯ�� �� �����ϴ� ��쿡�� Session�� ���� ������ �� �ʿ� ���� closesocket�� ȣ��.
		if (closeCase == SessionCloseCase::SessionPoolEmpty)
		{
			closesocket(socket);
			return;
		}

		// ������ ������ Ȱ��ȭ ���°� �ƴ϶�� �Է°� ������ ���� �ٷ� ����.
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		// ���� ó�� �Ŀ��� �Ϲ����� socket close
		closesocket(socket);

		// ���� Ǯ�� ������ ó���� ���ش�.
		_sessionPool[sessionIdx].Clear();
		_sessionPool.ReleaseTag(sessionIdx);

		// �������� ��Ŷť�� �־� �˷��ش�.
		addToPacketQueue(sessionIdx, (short)NetworkErrorCode::NotifyCloseSession, 0, nullptr);
	}

	// ���ú� ��Ŷ ť�� ������ ������ ��Ŷ�� �־��ִ� �Լ�.
	void NetworkMessenger::addToPacketQueue(const int sessionIdx, const short pktId, const short bodySize, char * body)
	{
		auto packetInfo = std::make_shared<PacketInfo>();
		packetInfo->_sessionIdx = sessionIdx;
		packetInfo->_packetId = pktId;
		packetInfo->_bodySize = bodySize;
		packetInfo->_body = body;

		_recvQueue->Push(packetInfo);
	}

	void NetworkMessenger::workerThreadFunc()
	{
	}

	void NetworkMessenger::listenThreadFunc()
	{
	}

	void NetworkMessenger::sendThreadFunc()
	{
	}

}
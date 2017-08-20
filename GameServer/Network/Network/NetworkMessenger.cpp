#include "NetworkMessenger.h"

#include <memory>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/ObjectPool.h"
#include "../../Common/Packet.h"

#include "IOInfo.h"
#include "SessionInfo.h"
#include "ServerNetworkErrorCode.h"

namespace FPNetwork
{
	using PacketHeader = FPCommon::PacketHeader;

	void NetworkMessenger::Stop()
	{
		WSACleanup();

		// ���� Ǯ Ŭ������ �ʱ�ȭ.
		_sessionPool.Release();
	}

	void NetworkMessenger::ForcingClose(const int sessionIdx)
	{
		// �������� ������ Ȱ��ȭ �Ǿ� ���� ���� ���¸� �׳� �����Ѵ�.
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		closeSession(
			SessionCloseCase::ForcingClose,
			static_cast<SOCKET>(_sessionPool[sessionIdx]._socket),
			sessionIdx);
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
		auto listenThread = std::thread(std::bind(&NetworkMessenger::listenThreadFunc, this));
		listenThread.detach();

		// �ý��� ������ �˾ƿ´�.
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int workerNum = si.dwNumberOfProcessors * 2;

		// �ھ� ���� �� �� ��ŭ worker �����带 Ȱ��ȭ�Ѵ�.
		for (auto i = 0; i < workerNum; ++i)
		{
			auto workerThread = std::thread(std::bind(&NetworkMessenger::workerThreadFunc, this));
			workerThread.detach();
		}

		// send �����带 Ȱ��ȭ �Ѵ�. 
		auto sendThread = std::thread(std::bind(&NetworkMessenger::sendThreadFunc, this));
		sendThread.detach();

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
		DWORD transferredByte = 0;
		IOInfo * ioInfo = nullptr;
		// Key�� �Ѿ� �´ٰ� �ϴµ�, ���� �𸣰ڰ� �Ⱦ�. ���߿� �˻��غ���¡. :)
		SessionInfo * key = nullptr;

		while (true)
		{
			auto retval = GetQueuedCompletionStatus(_iocpHandle, &transferredByte, (PULONG_PTR)&key, (LPOVERLAPPED*)&ioInfo, INFINITE);
			if (retval == FALSE)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Iocp GetQueuedCompletionStatus Failed", __FUNCTION__);
				continue;
			}

			auto sessionTag = ioInfo->SessionTag;
			auto& session = _sessionPool[sessionTag];

			_logger->Write(LogType::LOG_DEBUG, "%s | Socket FD(%I64u), Session(%d) request complete", __FUNCTION__, session._socket, sessionTag);

			if (ioInfo->Status == IOInfoStatus::READ)
			{
				// ���� �˻�.
				if (transferredByte == 0)
				{
					_logger->Write(LogType::LOG_INFO, "Socket FD(%I64u), Session(%d) connect ended", session._socket, sessionTag);
					session.Clear();
					_sessionPool.ReleaseTag(sessionTag);

					std::shared_ptr<PacketInfo> closeSessionInfo = std::make_shared<PacketInfo>();
					closeSessionInfo->_packetId= (short)NetworkErrorCode::NotifyCloseSession;
					closeSessionInfo->_sessionIdx = sessionTag;
					_recvQueue->Push(closeSessionInfo);

					continue;
				}

				auto headerPosition = session._recvBuffer;
				auto receivePosition = ioInfo->Wsabuf.buf;

				// ó���ȵ� �������� �� ��
				auto totalDataSize = receivePosition + transferredByte - session._recvBuffer;

				// ��Ŷ���� ��������� ��ٸ��� �������� ������
				auto remainDataSize = totalDataSize;

				const auto packetHeaderSize = FPCommon::packetHeaderSize;
				while (remainDataSize >= packetHeaderSize)
				{
					// ����� �鿩�� ���⿡ ����� �����Ͱ� �ִٸ� ����� �鿩�ٺ���.
					auto header = (PacketHeader*)headerPosition;
					auto bodySize = header->_bodySize;

					if (packetHeaderSize + bodySize >= remainDataSize)
					{
						// ��Ŷ�� ������ش�.
						std::shared_ptr<PacketInfo> newPacket = std::make_shared<PacketInfo>();
						newPacket->_packetId = header->_id;
						newPacket->_bodySize = bodySize;
						newPacket->_body= headerPosition + packetHeaderSize;
						newPacket->_sessionIdx = ioInfo->SessionTag;

						_recvQueue->Push(newPacket);
						_logger->Write(LogType::LOG_DEBUG, "%s | Making New Packet ID(%d), BodySize(%d), Session(%d)",
							__FUNCTION__,
							header->_id,
							bodySize,
							ioInfo->SessionTag);

						// ��Ŷ�� ���� ��, ���� �� ��� �ڸ��� �����ϰ�, ���� ������ ����� �����Ѵ�.
						headerPosition += packetHeaderSize + bodySize;
						remainDataSize -= packetHeaderSize + bodySize;
					}
					else
					{
						break;
					}
				}

				// ���� �����͸� ������ �� ������ ����ش�.
				memcpy_s(session._recvBuffer, _serverConfig._maxClientRecvSize, headerPosition, remainDataSize);

				// ���� �� �ִ� ��Ŷ�� �� ��������Ƿ�, Recv�� �Ǵ�.
				ZeroMemory(&ioInfo->Overlapped, sizeof(OVERLAPPED));

				// remainDataSize��ŭ�� ��� �޴´�.
				ioInfo->Wsabuf.buf = session._recvBuffer + remainDataSize;
				ioInfo->Wsabuf.len = _serverConfig._maxClientRecvSize - remainDataSize;
				ioInfo->Status = IOInfoStatus::READ;

				DWORD recvSize = 0;
				DWORD flags = 0;
				auto retval = WSARecv(session._socket, &ioInfo->Wsabuf, 1, &recvSize, &flags, &ioInfo->Overlapped, nullptr);

				if (SOCKET_ERROR == retval)
				{
					auto error = WSAGetLastError();
					if (error != WSA_IO_PENDING)
					{
						_logger->Write(LogType::LOG_ERROR, "%s | WSARecv Error(%d)", __FUNCTION__, error);
					}
				}
			}
			// TODO :: IOInfoStatus::WRITE�� �� ó��.
			else {}
		}
	}

	void NetworkMessenger::listenThreadFunc()
	{
		while (true)
		{
			SOCKADDR_IN clientAddr;
			int addrlen = sizeof(clientAddr);

			_logger->Write(LogType::LOG_DEBUG, "%s | Waiting For Other Client...", __FUNCTION__);

			// ���⼭ Blocking�Ǿ� �ٸ� Ŭ���̾�Ʈ�� accept�� ��ٸ���.
			SOCKET newClient = accept(_serverSocket, (SOCKADDR*)&clientAddr, &addrlen);
			if (newClient == INVALID_SOCKET)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Client accpet failed", __FUNCTION__);
				continue;
			}

			// Ǯ���� Session �ϳ��� �޾� ������ �������ش�.
			auto newTag = _sessionPool.GetTag();
			if (newTag < 0)
			{
				_logger->Write(LogType::LOG_WARN, "%s | Client Session Pool Full", __FUNCTION__);
				// TODO :: ���⼭ continue���� ������ �ִ��� ��� ó�����־�� ��.
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Client Accept, Socket FD(%I64u) Session(%d)", __FUNCTION__, _serverSocket, newTag);

			auto& newSession = _sessionPool[newTag];
			newSession._tag = newTag;
			newSession._socket = newClient;
			newSession._socketAddress = clientAddr;

			auto newIOCPInfo = new IOInfo();
			ZeroMemory(&newIOCPInfo->Overlapped, sizeof(OVERLAPPED));
			newIOCPInfo->Wsabuf.buf = newSession._recvBuffer;
			newIOCPInfo->Wsabuf.len = _serverConfig._maxClientRecvSize;
			newIOCPInfo->Status = IOInfoStatus::READ;
			newIOCPInfo->SessionTag = newTag;

			// IOCP�� ���ο� ������ ������ش�.
			CreateIoCompletionPort((HANDLE)&newSession._socket, _iocpHandle, (ULONG_PTR)nullptr, 0);

			DWORD recvSize = 0;
			DWORD flags = 0;

			// ���ú긦 �ɾ���´�.
			auto retval = WSARecv(
				newSession._socket,
				&newIOCPInfo->Wsabuf,
				1,
				&recvSize, &flags, &newIOCPInfo->Overlapped, nullptr);
			_logger->Write(LogType::LOG_DEBUG, "%s | Waiting for recv massage from socket(%I64u)", __FUNCTION__, newSession._socket);

			if (SOCKET_ERROR == retval)
			{
				auto error = WSAGetLastError();
				if (error != WSA_IO_PENDING)
				{
					_logger->Write(LogType::LOG_ERROR, "%s | WSARecv Error(%d)", __FUNCTION__, error);
				}
			}
		}
	}

	void NetworkMessenger::sendThreadFunc()
	{
		while (true)
		{
			// ���� ��Ŷ�� ���ٸ�,
			if (_sendQueue->IsEmpty())
			{
				// �纸�Ѵ�.
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_DEBUG, "%s | SendThreadFunc Entry", __FUNCTION__);

			std::shared_ptr<PacketInfo> sendPacket = _sendQueue->Peek();
			auto destSession = _sessionPool[sendPacket->_sessionIdx];
			auto sendHeader = PacketHeader{ sendPacket->_packetId, sendPacket->_bodySize };

			char* sendChar = (char*)&sendHeader;
			// TODO :: strcat_s�� ���� ���� ���� ����. �������� strcat���.
			strcat_s(sendChar, sizeof(sendChar), sendPacket->_body);

			send(destSession._socket, sendChar, FPCommon::packetHeaderSize + sendPacket->_bodySize, 0);

			//send(destSession._socket, (char*)&sendHeader, FirePlayCommon::packetHeaderSize, 0);
			//send(destSession._socket, sendPacket->pData, sendPacket->PacketBodySize, 0);

			_sendQueue->Pop();

			_logger->Write(LogType::LOG_DEBUG, "%s | Send Packet, To Socket(%I64u), Session(%d), Packet ID(%d)", __FUNCTION__, destSession._socket, destSession._tag, static_cast<int>(sendPacket->_packetId));
		}
	}

}
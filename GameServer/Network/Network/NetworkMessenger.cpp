#include "NetworkMessenger.h"

#include <memory>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/ObjectPool.h"
#include "../../Common/Packet.h"

#include "IOInfo.h"
#include "SessionInfo.h"
#include "PacketInfo.h"
#include "HttpNetwork.h"
#include "ServerNetworkErrorCode.h"

namespace FPNetwork
{
	using PacketHeader = FPNetwork::PacketHeader;

	void NetworkMessenger::Stop()
	{
		//ȡ������Ự���ࡣ
		_sessionPool.Release();

		WSACleanup();
	}

	void NetworkMessenger::ForcingClose(const int sessionIdx)
	{
		// ���Ҫ�رյĻỰδ�����ֻ���ء�
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		closeSession(
			SessionCloseCase::ForcingClose,
			static_cast<SOCKET>(_sessionPool[sessionIdx]._socket),
			sessionIdx);
	}

	bool NetworkMessenger::Init(
			ConsoleLogger * logger,
			ServerConfig * serverConfig,
			PacketQueue * recvQueue,
			PacketQueue * sendQueue)
	{
		if (logger       == nullptr ||
			serverConfig == nullptr ||
			recvQueue    == nullptr ||
			sendQueue    == nullptr )
		{
			return false;
		}

		_sendBuffer = new char[FPNetwork::packetHeaderSize + FPNetwork::maxPacketBodySize];

		_logger = logger;
		_recvQueue = recvQueue;
		_sendQueue = sendQueue;
		_httpNetwork = std::make_unique<HttpNetwork>();
		_httpNetwork->Init(_logger);

		memcpy(&_serverConfig, serverConfig, sizeof(ServerConfig));

		//��ʼ���ͻ��˻Ự�ء�
		_sessionPool.Init(_serverConfig._maxClientCount);
		for (auto i = 0; i < _serverConfig._maxClientCount; ++i)
		{
			_sessionPool[i]._tag = i;
			_sessionPool[i]._recvBuffer = new char[_serverConfig._maxClientRecvSize];
		}

		//�����׽��ֱ��ʹ�á�
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | WSAStartUp Failed", __FUNCTION__);
			return false;
		}

		//����IOCP��
		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		if (_iocpHandle == INVALID_HANDLE_VALUE)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | CreateIoCompletionPort Failed", __FUNCTION__);
			return false;
		}

		//����������׽��֡�
		_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (_serverSocket == INVALID_SOCKET)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | ServerSocket Failed", __FUNCTION__);
			return false;
		}

		//�����׽��ֺ�IOCP�˿ڡ�
		HANDLE hResult = CreateIoCompletionPort((HANDLE)_serverSocket, _iocpHandle, 0, 0);
		if (hResult != _iocpHandle)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket connection to IOCP Failed", __FUNCTION__);
			return false;
		}

		// �����ð󶨵��������׽��֡�
		sockaddr_in socketAddr;
		ZeroMemory(&socketAddr, sizeof(socketAddr));
		socketAddr.sin_family = AF_INET;
		socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		socketAddr.sin_port = htons(_serverConfig._port);
		auto result = bind(_serverSocket, (sockaddr*)&socketAddr, sizeof(socketAddr));
		if (result == SOCKET_ERROR)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket Bind Failed", __FUNCTION__);
			return false;
		}

		_logger->Write(LogType::LOG_INFO, "NetworkMessenger Create :: Port(%d), Backlog(%d)", _serverConfig._port, _serverConfig._backlog);

		//�г����õ��׽��֡�
		result = listen(_serverSocket, _serverConfig._backlog);
		//result = listen(_serverSocket, SOMAXCONN);
		if (result != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket listen Failed", __FUNCTION__);
			return false;
		}
		_logger->Write(LogType::LOG_INFO, "%s | Listen Start. ServerSocketFD(%I64u), BackLog(%d)", __FUNCTION__, _serverSocket, _serverConfig._backlog);


		//����Listen�̡߳�
		auto listenThread = std::thread(std::bind(&NetworkMessenger::listenThreadFunc, this));
		listenThread.detach();

		//��ȡϵͳ��Ϣ
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int workerNum = static_cast<int>(si.dwNumberOfProcessors * 2);

		//������߳����κ�������
		for (auto i = 0; i < workerNum; ++i)
		{
			auto workerThread = std::thread(std::bind(&NetworkMessenger::workerThreadFunc, this));
			workerThread.detach();
		}

		// //������̡߳�
		auto sendThread = std::thread(std::bind(&NetworkMessenger::sendThreadFunc, this));
		sendThread.detach();

		return true;
	}

	//�ر�ָ���Ự�Ĺ��ܡ�
	void NetworkMessenger::closeSession(const SessionCloseCase closeCase, const SOCKET socket, const int sessionIdx)
	{
		//����Ự��Ϊ�գ���ֻ�����closesocket����������Session��
		if (closeCase == SessionCloseCase::SessionPoolEmpty)
		{
			closesocket(socket);
			return;
		}

		//���ָ���ĻỰδ���ڻ״̬����Ὣ�䱨��Ϊ��������������ء�
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		//�쳣����������׽��ֹر�
		closesocket(socket);

		//����Ự�ء�
		_sessionPool[sessionIdx].Clear();
		_sessionPool.ReleaseTag(sessionIdx);

		//�������ݰ����еĽ�����
		addToPacketQueue(sessionIdx, (short)NetworkErrorCode::NotifyCloseSession, 0, nullptr);
	}

	//�������ݰ����ö�����ָ������Ϣ���Ĺ��ܡ�
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
		HANDLE ioCompletionPort = _iocpHandle;
		DWORD transferredByte = 0;
		IOInfo * ioInfo = nullptr;
		// Key˵�Ѿ������ˣ����Ҳ�֪����ʲô�����Ժ�������:)// Key˵�Ѿ������ˣ����Ҳ�֪����ʲô�����Ժ�������:)
		SessionInfo * key = nullptr;

		while (true)
		{
			auto retval = GetQueuedCompletionStatus(ioCompletionPort, &transferredByte, (PULONG_PTR)&key, (LPOVERLAPPED*)&ioInfo, INFINITE);
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
				//�˳����
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

				//δ�������ݵ�����
				auto totalDataSize = receivePosition + transferredByte - session._recvBuffer;

				//�ȴ��������ݰ������ݴ�С
				auto remainDataSize = totalDataSize;

				const auto packetHeaderSize = FPNetwork::packetHeaderSize;
				while (remainDataSize >= packetHeaderSize)
				{
					//������㹻���������鿴���⣬��鿴�����ڲ���
					auto header = (PacketHeader*)headerPosition;
					auto bodySize = header->_bodySize;

					long long requiredSize = packetHeaderSize + bodySize;
					if (requiredSize <= remainDataSize)
					{
						///����һ�����ݰ���
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

						//�������ݰ���ָ����һ������λ�ò�����ʣ�����ݴ�С��
						headerPosition += packetHeaderSize + bodySize;
						remainDataSize -= packetHeaderSize + bodySize;
					}
					else
					{
						break;
					}
				}

				//��ʣ������������������ǰ�档
				memcpy_s(session._recvBuffer, _serverConfig._maxClientRecvSize, headerPosition, remainDataSize);

				//���Ǵ��������п��Դ��������ݰ����������ͨ����Recv��
				ZeroMemory(&ioInfo->Overlapped, sizeof(OVERLAPPED));

				//������retainDataSize��
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

			//��ֹ����ȴ���һ���ͻ��Ľ��ܡ�
			SOCKET newClient = accept(_serverSocket, (SOCKADDR*)&clientAddr, &addrlen);
			if (newClient == INVALID_SOCKET)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Client accpet failed", __FUNCTION__);
				continue;
			}


			//�ӳ��л�ȡ�Ự����д��Ϣ��
			auto newTag = _sessionPool.GetTag();
			if (newTag < 0)
			{
				_logger->Write(LogType::LOG_WARN, "%s | Client Session Pool Full", __FUNCTION__);
				// TODO ::����������ϵ����������봦�����������Ǽ�����
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Client Accept, Socket FD(%I64u) Session(%d)", __FUNCTION__, _serverSocket, newTag);

			//ʹ�ô���Ϣ��д�Ự�ء�
			auto& newSession = _sessionPool[newTag];
			newSession._tag = newTag;
			newSession._socket = newClient;
			newSession._socketAddress = clientAddr;

			auto newIOCPInfo = new IOInfo();
			ZeroMemory(&(newIOCPInfo->Overlapped), sizeof(OVERLAPPED));
			newIOCPInfo->Wsabuf.buf = newSession._recvBuffer;
			newIOCPInfo->Wsabuf.len = _serverConfig._maxClientRecvSize;
			newIOCPInfo->Status = IOInfoStatus::READ;
			newIOCPInfo->SessionTag = newTag;

			//ʹ��IOCPע���»Ự��
			CreateIoCompletionPort((HANDLE)newSession._socket, _iocpHandle, (DWORD)newSession._socket, 0);

			DWORD recvSize = 0;
			DWORD flags = 0;

			//�ҶϽ�����
			auto retval = WSARecv(
				newSession._socket,
				&newIOCPInfo->Wsabuf,
				1,
				&recvSize, &flags, &(newIOCPInfo->Overlapped), nullptr);
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
			//���û��Ҫ���͵����ݰ���
			if (_sendQueue->IsEmpty())
			{
				//����
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			std::shared_ptr<PacketInfo> sendPacket = _sendQueue->Peek();
			auto destSession = _sessionPool[sendPacket->_sessionIdx];
			auto sendHeader = PacketHeader{ sendPacket->_packetId, sendPacket->_bodySize };

			memcpy_s(_sendBuffer, FPNetwork::packetHeaderSize + sendPacket->_bodySize, &sendHeader, FPNetwork::packetHeaderSize);
			strcpy_s(_sendBuffer + FPNetwork::packetHeaderSize, sendPacket->_bodySize, sendPacket->_body);

			auto sendByteSize = send(destSession._socket, _sendBuffer, FPNetwork::packetHeaderSize + sendPacket->_bodySize, 0);
			if (sendByteSize < 0)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Socket Send has Problem.", __FUNCTION__);
			}

			_sendQueue->Pop();

			_logger->Write(LogType::LOG_DEBUG, "%s | Send Packet, To Socket(%I64u), Session(%d), Packet ID(%d), BodySize(%d)", __FUNCTION__, destSession._socket, destSession._tag, static_cast<int>(sendPacket->_packetId), sendPacket->_bodySize);
		}
	}

}
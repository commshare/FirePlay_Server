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
		//取消分配会话池类。
		_sessionPool.Release();

		WSACleanup();
	}

	void NetworkMessenger::ForcingClose(const int sessionIdx)
	{
		// 如果要关闭的会话未激活，则只返回。
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

		//初始化客户端会话池。
		_sessionPool.Init(_serverConfig._maxClientCount);
		for (auto i = 0; i < _serverConfig._maxClientCount; ++i)
		{
			_sessionPool[i]._tag = i;
			_sessionPool[i]._recvBuffer = new char[_serverConfig._maxClientRecvSize];
		}

		//声明套接字编程使用。
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | WSAStartUp Failed", __FUNCTION__);
			return false;
		}

		//创建IOCP。
		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		if (_iocpHandle == INVALID_HANDLE_VALUE)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | CreateIoCompletionPort Failed", __FUNCTION__);
			return false;
		}

		//激活服务器套接字。
		_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (_serverSocket == INVALID_SOCKET)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | ServerSocket Failed", __FUNCTION__);
			return false;
		}

		//连接套接字和IOCP端口。
		HANDLE hResult = CreateIoCompletionPort((HANDLE)_serverSocket, _iocpHandle, 0, 0);
		if (hResult != _iocpHandle)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket connection to IOCP Failed", __FUNCTION__);
			return false;
		}

		// 将设置绑定到服务器套接字。
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

		//列出设置的套接字。
		result = listen(_serverSocket, _serverConfig._backlog);
		//result = listen(_serverSocket, SOMAXCONN);
		if (result != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket listen Failed", __FUNCTION__);
			return false;
		}
		_logger->Write(LogType::LOG_INFO, "%s | Listen Start. ServerSocketFD(%I64u), BackLog(%d)", __FUNCTION__, _serverSocket, _serverConfig._backlog);


		//激活Listen线程。
		auto listenThread = std::thread(std::bind(&NetworkMessenger::listenThreadFunc, this));
		listenThread.detach();

		//获取系统信息
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int workerNum = static_cast<int>(si.dwNumberOfProcessors * 2);

		//激活工作线程两次核心数。
		for (auto i = 0; i < workerNum; ++i)
		{
			auto workerThread = std::thread(std::bind(&NetworkMessenger::workerThreadFunc, this));
			workerThread.detach();
		}

		// //激活发送线程。
		auto sendThread = std::thread(std::bind(&NetworkMessenger::sendThreadFunc, this));
		sendThread.detach();

		return true;
	}

	//关闭指定会话的功能。
	void NetworkMessenger::closeSession(const SessionCloseCase closeCase, const SOCKET socket, const int sessionIdx)
	{
		//如果会话池为空，则只需调用closesocket而无需设置Session。
		if (closeCase == SessionCloseCase::SessionPoolEmpty)
		{
			closesocket(socket);
			return;
		}

		//如果指定的会话未处于活动状态，则会将其报告为输入错误并立即返回。
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		//异常处理后，正常套接字关闭
		closesocket(socket);

		//处理会话池。
		_sessionPool[sessionIdx].Clear();
		_sessionPool.ReleaseTag(sessionIdx);

		//告诉数据包队列的结束。
		addToPacketQueue(sessionIdx, (short)NetworkErrorCode::NotifyCloseSession, 0, nullptr);
	}

	//接收数据包放置队列中指定的信息包的功能。
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
		// Key说已经结束了，但我不知道是什么。请稍后搜索。:)// Key说已经结束了，但我不知道是什么。请稍后搜索。:)
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
				//退出检查
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

				//未处理数据的总量
				auto totalDataSize = receivePosition + transferredByte - session._recvBuffer;

				//等待进入数据包的数据大小
				auto remainDataSize = totalDataSize;

				const auto packetHeaderSize = FPNetwork::packetHeaderSize;
				while (remainDataSize >= packetHeaderSize)
				{
					//如果有足够的数据来查看标题，请查看标题内部。
					auto header = (PacketHeader*)headerPosition;
					auto bodySize = header->_bodySize;

					long long requiredSize = packetHeaderSize + bodySize;
					if (requiredSize <= remainDataSize)
					{
						///创建一个数据包。
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

						//创建数据包后，指定下一个标题位置并更新剩余数据大小。
						headerPosition += packetHeaderSize + bodySize;
						remainDataSize -= packetHeaderSize + bodySize;
					}
					else
					{
						break;
					}
				}

				//将剩余数据拉到缓冲区的前面。
				memcpy_s(session._recvBuffer, _serverConfig._maxClientRecvSize, headerPosition, remainDataSize);

				//我们创建了所有可以创建的数据包，因此我们通过了Recv。
				ZeroMemory(&ioInfo->Overlapped, sizeof(OVERLAPPED));

				//将生成retainDataSize。
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
			// TODO :: IOInfoStatus::WRITE老 锭 贸府.
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

			//阻止这里等待另一个客户的接受。
			SOCKET newClient = accept(_serverSocket, (SOCKADDR*)&clientAddr, &addrlen);
			if (newClient == INVALID_SOCKET)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Client accpet failed", __FUNCTION__);
				continue;
			}


			//从池中获取会话并填写信息。
			auto newTag = _sessionPool.GetTag();
			if (newTag < 0)
			{
				_logger->Write(LogType::LOG_WARN, "%s | Client Session Pool Full", __FUNCTION__);
				// TODO ::如果是最大联系人数，则必须处理它，而不是继续。
				continue;
			}

			_logger->Write(LogType::LOG_INFO, "%s | Client Accept, Socket FD(%I64u) Session(%d)", __FUNCTION__, _serverSocket, newTag);

			//使用此信息填写会话池。
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

			//使用IOCP注册新会话。
			CreateIoCompletionPort((HANDLE)newSession._socket, _iocpHandle, (DWORD)newSession._socket, 0);

			DWORD recvSize = 0;
			DWORD flags = 0;

			//挂断接收器
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
			//如果没有要发送的数据包，
			if (_sendQueue->IsEmpty())
			{
				//放弃
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
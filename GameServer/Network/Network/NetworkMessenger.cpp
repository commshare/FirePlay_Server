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

		// 세션 풀 클래스를 초기화.
		_sessionPool.Release();
	}

	void NetworkMessenger::ForcingClose(const int sessionIdx)
	{
		// 닫으려는 세션이 활성화 되어 있지 않은 상태면 그냥 리턴한다.
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

		// 클라이언트 세션 풀 초기화.
		_sessionPool.Init(_serverConfig._maxClientCount);
		for (int i = 0; i < _serverConfig._maxClientCount; ++i)
		{
			_sessionPool[i]._tag = i;
			_sessionPool[i]._recvBuffer = new char[_serverConfig._maxClientRecvSize];
		}

		// 소켓 프로그래밍 사용 선언.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | WSAStartUp Failed", __FUNCTION__);
			return false;
		}

		// IOCP를 생성한다.
		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		if (_iocpHandle == INVALID_HANDLE_VALUE)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | CreateIoCompletionPort Failed", __FUNCTION__);
			return false;
		}

		// 서버 소켓을 활성화한다.
		_serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (_serverSocket == INVALID_SOCKET)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | ServerSocket Failed", __FUNCTION__);
			return false;
		}

		// 서버 소켓에 설정 값을 바인딩 한다.
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

		// 세팅된 소켓을 listen해준다.
		result = listen(_serverSocket, _serverConfig._backlog);
		if (result != 0)
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Socket listen Failed", __FUNCTION__);
			return false;
		}
		_logger->Write(LogType::LOG_INFO, "%s | Listen Start. ServerSocketFD(%I64u), BackLog(%d)", __FUNCTION__, _serverSocket, _serverConfig._backlog);

		// Listen 쓰레드를 활성화 한다.
		auto listenThread = std::thread(std::bind(&NetworkMessenger::listenThreadFunc, this));
		listenThread.detach();

		// 시스템 정보를 알아온다.
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int workerNum = si.dwNumberOfProcessors * 2;

		// 코어 수의 두 배 만큼 worker 쓰레드를 활성화한다.
		for (auto i = 0; i < workerNum; ++i)
		{
			auto workerThread = std::thread(std::bind(&NetworkMessenger::workerThreadFunc, this));
			workerThread.detach();
		}

		// send 쓰레드를 활성화 한다. 
		auto sendThread = std::thread(std::bind(&NetworkMessenger::sendThreadFunc, this));
		sendThread.detach();

		return true;
	}

	// 지정한 세션을 닫아주는 함수.
	void NetworkMessenger::closeSession(const SessionCloseCase closeCase, const SOCKET socket, const int sessionIdx)
	{
		// 세션 풀이 비어서 종료하는 경우에는 Session을 따로 설정해 줄 필요 없이 closesocket만 호출.
		if (closeCase == SessionCloseCase::SessionPoolEmpty)
		{
			closesocket(socket);
			return;
		}

		// 지정한 세션이 활성화 상태가 아니라면 입력값 오류로 보고 바로 리턴.
		if (_sessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		// 예외 처리 후에는 일반적인 socket close
		closesocket(socket);

		// 세션 풀에 관련한 처리를 해준다.
		_sessionPool[sessionIdx].Clear();
		_sessionPool.ReleaseTag(sessionIdx);

		// 끝났음을 패킷큐에 넣어 알려준다.
		addToPacketQueue(sessionIdx, (short)NetworkErrorCode::NotifyCloseSession, 0, nullptr);
	}

	// 리시브 패킷 큐에 지정한 정보의 패킷을 넣어주는 함수.
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
		// Key가 넘어 온다고 하는데, 뭔지 모르겠고 안씀. 나중에 검색해봐야징. :)
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
				// 종료 검사.
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

				// 처리안된 데이터의 총 량
				auto totalDataSize = receivePosition + transferredByte - session._recvBuffer;

				// 패킷으로 만들어지길 기다리는 데이터의 사이즈
				auto remainDataSize = totalDataSize;

				const auto packetHeaderSize = FPCommon::packetHeaderSize;
				while (remainDataSize >= packetHeaderSize)
				{
					// 헤더를 들여다 보기에 충분한 데이터가 있다면 헤더를 들여다본다.
					auto header = (PacketHeader*)headerPosition;
					auto bodySize = header->_bodySize;

					if (packetHeaderSize + bodySize >= remainDataSize)
					{
						// 패킷을 만들어준다.
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

						// 패킷을 만든 후, 다음 번 헤더 자리를 지정하고, 남은 데이터 사이즈를 갱신한다.
						headerPosition += packetHeaderSize + bodySize;
						remainDataSize -= packetHeaderSize + bodySize;
					}
					else
					{
						break;
					}
				}

				// 남은 데이터를 버퍼의 맨 앞으로 당겨준다.
				memcpy_s(session._recvBuffer, _serverConfig._maxClientRecvSize, headerPosition, remainDataSize);

				// 만들 수 있는 패킷은 다 만들었으므로, Recv를 건다.
				ZeroMemory(&ioInfo->Overlapped, sizeof(OVERLAPPED));

				// remainDataSize만큼은 띄고 받는다.
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
			// TODO :: IOInfoStatus::WRITE일 때 처리.
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

			// 여기서 Blocking되어 다른 클라이언트의 accept를 기다린다.
			SOCKET newClient = accept(_serverSocket, (SOCKADDR*)&clientAddr, &addrlen);
			if (newClient == INVALID_SOCKET)
			{
				_logger->Write(LogType::LOG_ERROR, "%s | Client accpet failed", __FUNCTION__);
				continue;
			}

			// 풀에서 Session 하나를 받아 정보를 기입해준다.
			auto newTag = _sessionPool.GetTag();
			if (newTag < 0)
			{
				_logger->Write(LogType::LOG_WARN, "%s | Client Session Pool Full", __FUNCTION__);
				// TODO :: 여기서 continue말고 동접자 최대일 경우 처리해주어야 함.
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

			// IOCP에 새로운 세션을 등록해준다.
			CreateIoCompletionPort((HANDLE)&newSession._socket, _iocpHandle, (ULONG_PTR)nullptr, 0);

			DWORD recvSize = 0;
			DWORD flags = 0;

			// 리시브를 걸어놓는다.
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
			// 보낼 패킷이 없다면,
			if (_sendQueue->IsEmpty())
			{
				// 양보한다.
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}

			_logger->Write(LogType::LOG_DEBUG, "%s | SendThreadFunc Entry", __FUNCTION__);

			std::shared_ptr<PacketInfo> sendPacket = _sendQueue->Peek();
			auto destSession = _sessionPool[sendPacket->_sessionIdx];
			auto sendHeader = PacketHeader{ sendPacket->_packetId, sendPacket->_bodySize };

			char* sendChar = (char*)&sendHeader;
			// TODO :: strcat_s에 에러 있을 수도 있음. 기존에는 strcat사용.
			strcat_s(sendChar, sizeof(sendChar), sendPacket->_body);

			send(destSession._socket, sendChar, FPCommon::packetHeaderSize + sendPacket->_bodySize, 0);

			//send(destSession._socket, (char*)&sendHeader, FirePlayCommon::packetHeaderSize, 0);
			//send(destSession._socket, sendPacket->pData, sendPacket->PacketBodySize, 0);

			_sendQueue->Pop();

			_logger->Write(LogType::LOG_DEBUG, "%s | Send Packet, To Socket(%I64u), Session(%d), Packet ID(%d)", __FUNCTION__, destSession._socket, destSession._tag, static_cast<int>(sendPacket->_packetId));
		}
	}

}
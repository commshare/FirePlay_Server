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

		// 시스템 정보를 알아온다.

		// 코어 수의 두 배 만큼 worker 쓰레드를 활성화한다.

		// send 쓰레드를 활성화 한다. 

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
	}

	void NetworkMessenger::listenThreadFunc()
	{
	}

	void NetworkMessenger::sendThreadFunc()
	{
	}

}
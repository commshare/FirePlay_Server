#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"

#include "../../Network/Network/PacketInfo.h"

#include "Define.h"

namespace FPLogic
{
	void PacketProcess::LoginReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��û ��Ŷ ������ ��´�.
		Packet::LoginReq loginReq;
		PacketUnpack(packet, &loginReq);

		// Http�� ����Ͽ� Validation���� Ȯ��.
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// ���� ��Ŷ�� �غ�.
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// ��ȿ�� ��û�̾��ٸ� �ش� ������ UserManager�� �߰�.
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: User�� ������ DB���� �޾ƿͼ� �������־�� ��.

		// �亯 ����.
		PushToSendQueue(Packet::PacketId::ID_LoginRes, packet->_sessionIdx, &loginRes);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::FastMatchReq fastMatchReq;
		PacketUnpack(packet, &fastMatchReq);

		// ��û�� ������ ���¸� �������ش�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr || reqUser->IsUserActivated() == false)
		{
			// �̻��� ��û�� ���Դٰ� �����ϰ� �����Ѵ�.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Req Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}
		reqUser->JoinMatching(static_cast<CharacterType>(fastMatchReq._type), MatchingType::FastMatch);

		// ��û�� ������ �������� ��ġ ����Ŀ�� �־��ش�.
		_matchMaker->RequestMatch(packet->_sessionIdx);

		// ����� ��ȯ�Ѵ�.
		Packet::FastMatchRes matchRes;
		matchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FastMatchRes, packet->_sessionIdx, &matchRes);
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
		
		// ��Ŷ ������ ��´�.
		Packet::MatchCancelReq matchCancelReq;
		PacketUnpack(packet, &matchCancelReq);

		// ��û�� ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ��û�� ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.
		if (reqUser == nullptr || reqUser->IsUserActivated() == false)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Cancel Match Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ��û�� ������ ��ġ����Ŀ���� ���ش�.
		_matchMaker->CancleMatch(packet->_sessionIdx);

		// ��û�� ������ ���¸� ��ȭ��Ų��.
		reqUser->CancelMatching();

		// ����� ��ȯ�Ѵ�.
		Packet::MatchCancelRes cancelMatchRes;
		cancelMatchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MatchCancelRes, packet->_sessionIdx, &cancelMatchRes);
	}

	void PacketProcess::MatchSuccessAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��ġ�� Ȯ���� ������ ã�´�.

		// ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.

		// ��ġ�� Ȯ���� ������ ���� �뿡 �����ش�.

		// ����� ��ȯ�Ѵ�.
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::TurnStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::EnemyTurnStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::MoveNotify(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::EnemyMoveAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::FireNotify(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::EnemyFireAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::GameSetAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::CloseReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}
}
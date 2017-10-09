#include "PacketProcess.h"

#include <memory>

#include "../../include/json/json.h"

#include "../../Common/Packet.h"

#include "../../Network/Network/PacketInfo.h"

#include "Define.h"
#include "User.h"

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
		if (reqUser == nullptr)
		{
			// ��û�� ������ ���ٸ� �̻��� ��û�� ���Դٰ� �����ϰ� �����Ѵ�.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Cancel Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ��û�� ������ ��Īť�� �ִٸ� ��Īť���� ���ش�.
		if (_matchMaker->isUserIsInMatching(packet->_sessionIdx))
		{
			_matchMaker->CancleMatch(packet->_sessionIdx);
		}

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

		// ��Ŷ ������ ��´�.
		Packet::MatchSuccessAck matchAck;
		PacketUnpack(packet, &matchAck);

		// ��ġ�� Ȯ���� ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ������ GameRoom�� �ִ� �������� Ȯ���Ѵ�.
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ������ ���� �뿡 ��������ش�.		
		_gameRoomManager->EnterUserToRoom(reqUser->GetSessionIdx(), reqUser->GetGameIdx());
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::GameStartAck gameAck;
		PacketUnpack(packet, &gameAck);

		// ���� ������ Ȯ���� ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Game Start Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ������ GameRoom�� �ִ� �������� Ȯ���Ѵ�.
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ���� �뿡 ������ Ȯ���ߴٴ� �� �˷��ش�.
		// TODO :: �����ڵ� �޾Ƽ� �� Ack ���־����� Ȯ��.
		_gameRoomManager->GameStartAck(reqUser->GetGameIdx());
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

		// ��Ŷ ������ ��´�.
		Packet::MoveNotify moveNtf;
		PacketUnpack(packet, &moveNtf);

		// ������ ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Move Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ������ ���� ã�´�.
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ������ �����ش�.
		Packet::MoveAck moveAck;
		moveAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MoveAck, reqUser->GetSessionIdx(), &moveAck);

		// ������� �������� �����ش�.
		Packet::EnemyMoveNotify enemyMoveNotify;

		enemyMoveNotify._enemyPositionX = moveNtf._enemyPositionX;
		enemyMoveNotify._enemyPositionY = moveNtf._enemyPositionY;

		_logger->Write(LogType::LOG_DEBUG, "%s | Pos X : %d, Y : %d", __FUNCTION__, enemyMoveNotify._enemyPositionX, enemyMoveNotify._enemyPositionY);

		PushToSendQueue(Packet::PacketId::ID_EnemyMoveNotify, room->GetAnotherPlayerSession(reqUser->GetSessionIdx()), &enemyMoveNotify);
	}

	void PacketProcess::EnemyMoveAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
	}

	void PacketProcess::FireNotify(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::FireNotify fireNotify;
		PacketUnpack(packet, &fireNotify);

		// �߻��� ������ ã�´�.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ������ INVALID�ϴٸ� �α׸� ��� �����Ѵ�.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Fire Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ������ ���� ã�´�.
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ������ �����ش�.
		Packet::FireAck fireAck;
		fireAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FireAck, reqUser->GetSessionIdx(), &fireAck);

		// ������� �˷��ش�.
		Packet::EnemyFireNotify enemyFireNotify;
		enemyFireNotify._enemyPositionX = fireNotify._enemyPositionX;
		enemyFireNotify._fireType = fireNotify._fireType;
		enemyFireNotify._damage = fireNotify._damage;
		enemyFireNotify._magnitude = fireNotify._magnitude;
		enemyFireNotify._unitVecX = fireNotify._unitVecX;
		enemyFireNotify._unitVecY = fireNotify._unitVecY;

		PushToSendQueue(Packet::PacketId::ID_EnemyFireNotify, room->GetAnotherPlayerSession(reqUser->GetSessionIdx()), &enemyFireNotify);

		// �ش� ���� ���� �ٲ��ش�.
		_gameRoomManager->TurnChange(reqUser->GetGameIdx());
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

	void PacketProcess::TurnEndNofity(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::TurnEndNotify endNotify;
		PacketUnpack(packet, &endNotify);

		// ���� ������ ������ ã�´�.
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Turn End Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �ش� ���� ���� ã�´�.
		auto room = _gameRoomManager->GetRoom(notifyUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ������ �����ش�.
		Packet::TurnEndAck endAck;
		PushToSendQueue(Packet::PacketId::ID_TurnEndAck, notifyUser->GetSessionIdx(), &endAck);

		// �ش� ���� ���� �ٲ��ش�.
		_gameRoomManager->TurnChange(notifyUser->GetGameIdx());
	}

	void PacketProcess::DamageOccur(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��Ŷ ������ ��´�.
		Packet::DamageOccur healthInfo;
		PacketUnpack(packet, &healthInfo);
		
		// ��Ŷ�� ���� ������ ã�´�.
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Damage Occur Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ���� �� �����ڿ��� ������ �Ѱ��ش�.
		_gameRoomManager->SetDamageInfo(notifyUser->GetGameIdx(), healthInfo._player1Hp, healthInfo._player2Hp);
	}

}
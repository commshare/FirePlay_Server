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

		//��ȡ�������Ϣ��
		Packet::LoginReq loginReq;
		PacketUnpack(packet, &loginReq);

		// ��HTTP��ͨ����֤��֤�� 
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// �ڽ����豸�ڼ䡣
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// ����Ự����Ч�����򽫻Ự��ӵ�UserManager��
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: ��DB ��ȡ TODO :: User ���������������ǡ�

		//���ݴ�
		PushToSendQueue(Packet::PacketId::ID_LoginRes, packet->_sessionIdx, &loginRes);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//��ȡ���ݰ���Ϣ��
		Packet::FastMatchReq fastMatchReq;
		PacketUnpack(packet, &fastMatchReq);

		// ���������û���״̬��
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr || reqUser->IsUserActivated() == false)
		{
			// ����Ϊ��ֵ�����������������ǡ�
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Req Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}
		reqUser->JoinMatching(static_cast<CharacterType>(fastMatchReq._type), MatchingType::FastMatch);

		// �����������Ϣ�������matchmaker�С�
		_matchMaker->RequestMatch(packet->_sessionIdx);

		// ���ؽ��
		Packet::FastMatchRes matchRes;
		matchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FastMatchRes, packet->_sessionIdx, &matchRes);
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
		// ��ȡ���ݰ���Ϣ��
		Packet::MatchCancelReq matchCancelReq;
		PacketUnpack(packet, &matchCancelReq);

		//�ҵ�������û���
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr)
		{
			//���û���û�������ٶ����ղ�������һ����ֵ�����
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Cancel Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ��������û���ƥ������У����ƥ������м�ȥ����
		if (_matchMaker->isUserIsInMatching(packet->_sessionIdx))
		{
			_matchMaker->CancleMatch(packet->_sessionIdx);
		}

		// ���������û���״̬��
		reqUser->CancelMatching();

		// ���ؽ��
		Packet::MatchCancelRes cancelMatchRes;
		cancelMatchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MatchCancelRes, packet->_sessionIdx, &cancelMatchRes);
	}

	void PacketProcess::MatchSuccessAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//��ȡ���ݰ���Ϣ����.
		Packet::MatchSuccessAck matchAck;
		PacketUnpack(packet, &matchAck);

		// ��ȡ���ݰ���Ϣ��
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ����û���INVALID�����¼�����ԡ�
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ȷ���û���GameRoom�С�
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �����û�������Ϸ�ҡ�			
		_gameRoomManager->EnterUserToRoom(reqUser->GetSessionIdx(), reqUser->GetGameIdx());
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// ��ȡ���ݰ���Ϣ��
		Packet::GameStartAck gameAck;
		PacketUnpack(packet, &gameAck);

		// �ҵ�ȷ����Ϸ��ʼ���û���
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ����û���INVALID�����¼�����ԡ�
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Game Start Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ȷ���û���GameRoom�С�
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �����û���Ϸ�Ѿ�����Ϸ���н����˼�顣
		//TODO ::������Ƿ��յ�������벢��û���⡣
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

		// ��ȡ���ݰ���Ϣ��
		Packet::MoveNotify moveNtf;
		PacketUnpack(packet, &moveNtf);

		// �ҵ��ƶ����û���
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// ����û���INVALID�����¼�����ԡ�
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Move Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �ҵ��û��ķ���
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ���ͻظ���
		Packet::MoveAck moveAck;
		moveAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MoveAck, reqUser->GetSessionIdx(), &moveAck);

		// ���ƶ����͸����֡�
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

		//�չ�������
		Packet::FireNotify fireNotify;
		PacketUnpack(packet, &fireNotify);

		// �ҵ�����͵��û���
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		//�û���INVALID�����¼�����ԡ�
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Fire Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �ҵ��û��ķ��䡣
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ���ͻظ���
		Packet::FireAck fireAck;
		fireAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FireAck, reqUser->GetSessionIdx(), &fireAck);

		// ���߶Է�
		Packet::EnemyFireNotify enemyFireNotify;
		enemyFireNotify._enemyPositionX = fireNotify._enemyPositionX;
		enemyFireNotify._fireType = fireNotify._fireType;
		enemyFireNotify._damage = fireNotify._damage;
		enemyFireNotify._magnitude = fireNotify._magnitude;
		enemyFireNotify._unitVecX = fireNotify._unitVecX;
		enemyFireNotify._unitVecY = fireNotify._unitVecY;

		PushToSendQueue(Packet::PacketId::ID_EnemyFireNotify, room->GetAnotherPlayerSession(reqUser->GetSessionIdx()), &enemyFireNotify);

		//�ı䷿���ת�䡣
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

		// ��ȡ���ݰ���Ϣ��
		Packet::TurnEndNotify endNotify;
		PacketUnpack(packet, &endNotify);

		// �ҵ����ת����û���
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Turn End Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// �ҵ����ʵ���Ϸ�ҡ�
		auto room = _gameRoomManager->GetRoom(notifyUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// ���ͻظ���
		Packet::TurnEndAck endAck;
		PushToSendQueue(Packet::PacketId::ID_TurnEndAck, notifyUser->GetSessionIdx(), &endAck);

		// �ı䷿���ת�䡣
		_gameRoomManager->TurnChange(notifyUser->GetGameIdx());
	}

	void PacketProcess::DamageOccur(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//��ȡ���ݰ���Ϣ��
		Packet::DamageOccur healthInfo;
		PacketUnpack(packet, &healthInfo);
		
		// �ҵ��������ݰ����û���
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Damage Occur Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// ����Ϣ���ݸ���Ϸ�ҹ���Ա��
		_gameRoomManager->SetDamageInfo(notifyUser->GetGameIdx(), healthInfo._player1Hp, healthInfo._player2Hp);
	}

}
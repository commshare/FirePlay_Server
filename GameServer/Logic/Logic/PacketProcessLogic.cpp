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

		// 요청 패킷 정보를 얻는다.
		Packet::LoginReq loginReq;
		PacketUnpack(packet, &loginReq);

		// Http와 통신하여 Validation한지 확인.
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// 보낼 패킷을 준비.
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// 유효한 요청이었다면 해당 세션을 UserManager에 추가.
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: User의 전적을 DB에서 받아와서 갱신해주어야 함.

		// 답변 전달.
		PushToSendQueue(Packet::PacketId::ID_LoginRes, packet->_sessionIdx, &loginRes);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 패킷 정보를 얻는다.
		Packet::FastMatchReq fastMatchReq;
		PacketUnpack(packet, &fastMatchReq);

		// 요청한 유저의 상태를 변경해준다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr || reqUser->IsUserActivated() == false)
		{
			// 이상한 요청이 들어왔다고 생각하고 무시한다.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Req Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}
		reqUser->JoinMatching(static_cast<CharacterType>(fastMatchReq._type), MatchingType::FastMatch);

		// 요청한 정보를 바탕으로 매치 메이커에 넣어준다.
		_matchMaker->RequestMatch(packet->_sessionIdx);

		// 결과를 반환한다.
		Packet::FastMatchRes matchRes;
		matchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FastMatchRes, packet->_sessionIdx, &matchRes);
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
		// 패킷 정보를 얻는다.
		Packet::MatchCancelReq matchCancelReq;
		PacketUnpack(packet, &matchCancelReq);

		// 요청한 유저를 찾는다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr)
		{
			// 요청한 유저가 없다면 이상한 요청이 들어왔다고 생각하고 무시한다.
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Cancel Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 요청한 유저가 매칭큐에 있다면 매칭큐에서 빼준다.
		if (_matchMaker->isUserIsInMatching(packet->_sessionIdx))
		{
			_matchMaker->CancleMatch(packet->_sessionIdx);
		}

		// 요청한 유저의 상태를 변화시킨다.
		reqUser->CancelMatching();

		// 결과를 반환한다.
		Packet::MatchCancelRes cancelMatchRes;
		cancelMatchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MatchCancelRes, packet->_sessionIdx, &cancelMatchRes);
	}

	void PacketProcess::MatchSuccessAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 패킷 정보를 얻는다.
		Packet::MatchSuccessAck matchAck;
		PacketUnpack(packet, &matchAck);

		// 매치를 확인한 유저를 찾는다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 유저가 INVALID하다면 로그를 찍고 무시한다.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 유저가 GameRoom에 있는 상태인지 확인한다.
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 유저를 게임 룸에 입장시켜준다.		
		_gameRoomManager->EnterUserToRoom(reqUser->GetSessionIdx(), reqUser->GetGameIdx());
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 패킷 정보를 얻는다.
		Packet::GameStartAck gameAck;
		PacketUnpack(packet, &gameAck);

		// 게임 시작을 확인한 유저를 찾는다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 유저가 INVALID하다면 로그를 찍고 무시한다.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Game Start Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 유저가 GameRoom에 있는 상태인지 확인한다.
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 게임 룸에 유저가 확인했다는 걸 알려준다.
		// TODO :: 에러코드 받아서 잘 Ack 해주었는지 확인.
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

		// 패킷 정보를 얻는다.
		Packet::MoveNotify moveNtf;
		PacketUnpack(packet, &moveNtf);

		// 움직인 유저를 찾는다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 유저가 INVALID하다면 로그를 찍고 무시한다.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Move Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 유저의 방을 찾는다.
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 응답을 보내준다.
		Packet::MoveAck moveAck;
		moveAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MoveAck, reqUser->GetSessionIdx(), &moveAck);

		// 상대편에게 움직임을 보내준다.
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

		// 패킷 정보를 얻는다.
		Packet::FireNotify fireNotify;
		PacketUnpack(packet, &fireNotify);

		// 발사한 유저를 찾는다.
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 유저가 INVALID하다면 로그를 찍고 무시한다.
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Fire Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 유저의 방을 찾는다.
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 응답을 보내준다.
		Packet::FireAck fireAck;
		fireAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FireAck, reqUser->GetSessionIdx(), &fireAck);

		// 상대편에게 알려준다.
		Packet::EnemyFireNotify enemyFireNotify;
		enemyFireNotify._enemyPositionX = fireNotify._enemyPositionX;
		enemyFireNotify._fireType = fireNotify._fireType;
		enemyFireNotify._damage = fireNotify._damage;
		enemyFireNotify._magnitude = fireNotify._magnitude;
		enemyFireNotify._unitVecX = fireNotify._unitVecX;
		enemyFireNotify._unitVecY = fireNotify._unitVecY;

		PushToSendQueue(Packet::PacketId::ID_EnemyFireNotify, room->GetAnotherPlayerSession(reqUser->GetSessionIdx()), &enemyFireNotify);

		// 해당 방의 턴을 바꿔준다.
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

		// 패킷 정보를 얻는다.
		Packet::TurnEndNotify endNotify;
		PacketUnpack(packet, &endNotify);

		// 턴을 종료한 유저를 찾는다.
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Turn End Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 해당 게임 방을 찾는다.
		auto room = _gameRoomManager->GetRoom(notifyUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 응답을 보내준다.
		Packet::TurnEndAck endAck;
		PushToSendQueue(Packet::PacketId::ID_TurnEndAck, notifyUser->GetSessionIdx(), &endAck);

		// 해당 방의 턴을 바꿔준다.
		_gameRoomManager->TurnChange(notifyUser->GetGameIdx());
	}

	void PacketProcess::DamageOccur(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 패킷 정보를 얻는다.
		Packet::DamageOccur healthInfo;
		PacketUnpack(packet, &healthInfo);
		
		// 패킷을 보낸 유저를 찾는다.
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Damage Occur Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 게임 룸 관리자에게 정보를 넘겨준다.
		_gameRoomManager->SetDamageInfo(notifyUser->GetGameIdx(), healthInfo._player1Hp, healthInfo._player2Hp);
	}

}
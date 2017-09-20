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
		_gameRoomManager->GameStartAck(packet->_sessionIdx);
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
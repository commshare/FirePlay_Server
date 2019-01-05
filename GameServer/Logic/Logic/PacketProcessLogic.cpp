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

		//获取请求包信息。
		Packet::LoginReq loginReq;
		PacketUnpack(packet, &loginReq);

		// 在HTTP与通信验证验证。 
		auto result = _network->GetHttp()->PostTokenValidationRequest(loginReq._id, loginReq._token);

		// 在交付设备期间。
		Packet::LoginRes loginRes;
		loginRes._result = static_cast<int>(result);

		// 如果会话是有效请求，则将会话添加到UserManager。
		if (result == ErrorCode::None)
		{
			auto isLoginSuccessd = _userManager->LoginUser(packet->_sessionIdx, loginReq._id, loginReq._token);
			if (isLoginSuccessd == false)
			{
				loginRes._result = static_cast<int>(ErrorCode::NoRoomInUserPool);
			}
		}

		// TODO :: 从DB 获取 TODO :: User 的总数并更新它们。

		//传递答案
		PushToSendQueue(Packet::PacketId::ID_LoginRes, packet->_sessionIdx, &loginRes);
	}

	void PacketProcess::FastMatchReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//获取数据包信息。
		Packet::FastMatchReq fastMatchReq;
		PacketUnpack(packet, &fastMatchReq);

		// 更改请求用户的状态。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr || reqUser->IsUserActivated() == false)
		{
			// 我认为奇怪的请求进来并忽略它们。
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Req Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}
		reqUser->JoinMatching(static_cast<CharacterType>(fastMatchReq._type), MatchingType::FastMatch);

		// 根据请求的信息将其放入matchmaker中。
		_matchMaker->RequestMatch(packet->_sessionIdx);

		// 返回结果
		Packet::FastMatchRes matchRes;
		matchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FastMatchRes, packet->_sessionIdx, &matchRes);
	}

	void PacketProcess::MatchCancelReq(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);
		// 获取数据包信息。
		Packet::MatchCancelReq matchCancelReq;
		PacketUnpack(packet, &matchCancelReq);

		//找到请求的用户。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);
		if (reqUser == nullptr)
		{
			//如果没有用户请求，则假定接收并忽略了一个奇怪的请求。
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Cancel Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 如果请求用户在匹配队列中，则从匹配队列中减去它。
		if (_matchMaker->isUserIsInMatching(packet->_sessionIdx))
		{
			_matchMaker->CancleMatch(packet->_sessionIdx);
		}

		// 更改请求用户的状态。
		reqUser->CancelMatching();

		// 返回结果
		Packet::MatchCancelRes cancelMatchRes;
		cancelMatchRes._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MatchCancelRes, packet->_sessionIdx, &cancelMatchRes);
	}

	void PacketProcess::MatchSuccessAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//获取数据包信息。促.
		Packet::MatchSuccessAck matchAck;
		PacketUnpack(packet, &matchAck);

		// 获取数据包信息。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 如果用户是INVALID，请记录并忽略。
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Match Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 确保用户在GameRoom中。
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 允许用户进入游戏室。			
		_gameRoomManager->EnterUserToRoom(reqUser->GetSessionIdx(), reqUser->GetGameIdx());
	}

	void PacketProcess::GameStartAck(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		// 获取数据包信息。
		Packet::GameStartAck gameAck;
		PacketUnpack(packet, &gameAck);

		// 找到确认游戏开始的用户。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 如果用户是INVALID，请记录并忽略。
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Game Start Ack Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 确保用户在GameRoom中。
		if (reqUser->GetUserState() != UserState::InGame)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid User Matching Process, User is not in GameRoom, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 告诉用户游戏已经在游戏室中进行了检查。
		//TODO ::检查您是否收到错误代码并且没问题。
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

		// 获取数据包信息。
		Packet::MoveNotify moveNtf;
		PacketUnpack(packet, &moveNtf);

		// 找到移动的用户。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		// 如果用户是INVALID，请记录并忽略。
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Move Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 找到用户的房间
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 发送回复。
		Packet::MoveAck moveAck;
		moveAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_MoveAck, reqUser->GetSessionIdx(), &moveAck);

		// 将移动发送给对手。
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

		//收购数量。
		Packet::FireNotify fireNotify;
		PacketUnpack(packet, &fireNotify);

		// 找到被解雇的用户。
		auto reqUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		//用户是INVALID，请记录并忽略。
		if (reqUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Fire Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 找到用户的房间。
		auto room = _gameRoomManager->GetRoom(reqUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 发送回复。
		Packet::FireAck fireAck;
		fireAck._result = static_cast<int>(ErrorCode::None);

		PushToSendQueue(Packet::PacketId::ID_FireAck, reqUser->GetSessionIdx(), &fireAck);

		// 告诉对方
		Packet::EnemyFireNotify enemyFireNotify;
		enemyFireNotify._enemyPositionX = fireNotify._enemyPositionX;
		enemyFireNotify._fireType = fireNotify._fireType;
		enemyFireNotify._damage = fireNotify._damage;
		enemyFireNotify._magnitude = fireNotify._magnitude;
		enemyFireNotify._unitVecX = fireNotify._unitVecX;
		enemyFireNotify._unitVecY = fireNotify._unitVecY;

		PushToSendQueue(Packet::PacketId::ID_EnemyFireNotify, room->GetAnotherPlayerSession(reqUser->GetSessionIdx()), &enemyFireNotify);

		//改变房间的转弯。
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

		// 获取数据包信息。
		Packet::TurnEndNotify endNotify;
		PacketUnpack(packet, &endNotify);

		// 找到完成转弯的用户。
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Turn End Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 找到合适的游戏室。
		auto room = _gameRoomManager->GetRoom(notifyUser->GetGameIdx());
		if (room == nullptr || room->GetState() != RoomState::InGame)
		{
			return;
		}

		// 发送回复。
		Packet::TurnEndAck endAck;
		PushToSendQueue(Packet::PacketId::ID_TurnEndAck, notifyUser->GetSessionIdx(), &endAck);

		// 改变房间的转弯。
		_gameRoomManager->TurnChange(notifyUser->GetGameIdx());
	}

	void PacketProcess::DamageOccur(std::shared_ptr<PacketInfo> packet)
	{
		_logger->Write(LogType::LOG_DEBUG, "%s | Entry, Session(%d)", __FUNCTION__, packet->_sessionIdx);

		//获取数据包信息。
		Packet::DamageOccur healthInfo;
		PacketUnpack(packet, &healthInfo);
		
		// 找到发送数据包的用户。
		auto notifyUser = _userManager->FindUserWithSessionIdx(packet->_sessionIdx);

		if (notifyUser == nullptr)
		{
			_logger->Write(LogType::LOG_WARN, "%s | Invalid Damage Occur Ntf Input, Session Idx(%d)", __FUNCTION__, packet->_sessionIdx);
			return;
		}

		// 将信息传递给游戏室管理员。
		_gameRoomManager->SetDamageInfo(notifyUser->GetGameIdx(), healthInfo._player1Hp, healthInfo._player2Hp);
	}

}
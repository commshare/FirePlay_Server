#pragma once

#include <string>
#include <vector>
#include "..\GameServer\include\json\json.h"

namespace Packet
{
	class IJsonSerializable
	{
	public:
		virtual ~IJsonSerializable( void ) {};
		virtual void Serialize( Json::Value& root ) =0;
		virtual void Deserialize( Json::Value& root ) =0;
	};

	class CJsonSerializer
	{
	public:
		static bool Serialize( IJsonSerializable* pObj, std::string& output );
		static bool Deserialize( IJsonSerializable* pObj, std::string& input );

	private:
		CJsonSerializer( void ) {};
	};

	// C++ IOCP서버와 Unity C#이 통신하기 위한 패킷 정의 파일입니다.
	// 로그인 요청 패킷
	class LoginReq : public IJsonSerializable
	{
	public:
		LoginReq( void ) {}
		virtual ~LoginReq( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					_id;
		std::string					_token;
	};

	// 로그인 답변 패킷
	class LoginRes : public IJsonSerializable
	{
	public:
		LoginRes( void ) {}
		virtual ~LoginRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 빠른 매치 요청 패킷
	class FastMatchReq : public IJsonSerializable
	{
	public:
		FastMatchReq( void ) {}
		virtual ~FastMatchReq( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_type;
		std::string					_id;
		std::string					_token;
	};

	// 매치 요청 답변 패킷
	class FastMatchRes : public IJsonSerializable
	{
	public:
		FastMatchRes( void ) {}
		virtual ~FastMatchRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 매치 취소 패킷
	class MatchCancelReq : public IJsonSerializable
	{
	public:
		MatchCancelReq( void ) {}
		virtual ~MatchCancelReq( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					_id;
		std::string					_token;
	};

	// 매치 취소 답변 패킷
	class MatchCancelRes : public IJsonSerializable
	{
	public:
		MatchCancelRes( void ) {}
		virtual ~MatchCancelRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 매칭 성사 알림 패킷
	class MatchSuccessNotify : public IJsonSerializable
	{
	public:
		MatchSuccessNotify( void ) {}
		virtual ~MatchSuccessNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_gameNumber;
		std::string					_enemyId;
		int							_enemyWins;
		int							_enemyLoses;
		int							_enemyType;
	};

	// 매칭 성사 응답 패킷
	class MatchSuccessAck : public IJsonSerializable
	{
	public:
		MatchSuccessAck( void ) {}
		virtual ~MatchSuccessAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 게임 시작 알림 패킷
	class GameStartNotify : public IJsonSerializable
	{
	public:
		GameStartNotify( void ) {}
		virtual ~GameStartNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_playerNumber;
		float						_positionX;
		float						_enemyPositionX;
	};

	// 게임 시작 응답 패킷
	class GameStartAck : public IJsonSerializable
	{
	public:
		GameStartAck( void ) {}
		virtual ~GameStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 내 턴 시작 패킷
	class TurnStartNotify : public IJsonSerializable
	{
	public:
		TurnStartNotify( void ) {}
		virtual ~TurnStartNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		float						_windX;
		float						_windY;
	};

	// 내 턴 시작 응답 패킷
	class TurnStartAck : public IJsonSerializable
	{
	public:
		TurnStartAck( void ) {}
		virtual ~TurnStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 상대 턴 시작 패킷
	class EnemyTurnStartNotify : public IJsonSerializable
	{
	public:
		EnemyTurnStartNotify( void ) {}
		virtual ~EnemyTurnStartNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		float						_windX;
		float						_windY;
	};

	// 상대 턴 시작 응답 패킷
	class EnemyTurnStartAck : public IJsonSerializable
	{
	public:
		EnemyTurnStartAck( void ) {}
		virtual ~EnemyTurnStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 움직임 전달 패킷
	class MoveNotify : public IJsonSerializable
	{
	public:
		MoveNotify( void ) {}
		virtual ~MoveNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		float						_enemyPositionX;
		float						_enemyPositionY;
	};

	// 움직임 응답 패킷
	class MoveAck : public IJsonSerializable
	{
	public:
		MoveAck( void ) {}
		virtual ~MoveAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 상대 움직임 알림 패킷
	class EnemyMoveNotify : public IJsonSerializable
	{
	public:
		EnemyMoveNotify( void ) {}
		virtual ~EnemyMoveNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		float						_enemyPositionX;
		float						_enemyPositionY;
	};

	// 상대 움직임 응답 패킷
	class EnemyMoveAck : public IJsonSerializable
	{
	public:
		EnemyMoveAck( void ) {}
		virtual ~EnemyMoveAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 발사 패킷
	class FireNotify : public IJsonSerializable
	{
	public:
		FireNotify( void ) {}
		virtual ~FireNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_fireType;
		int							_damage;
		float						_enemyPositionX;
		float						_enemyPositionY;
		float						_unitVecX;
		float						_unitVecY;
		float						_magnitude;
	};

	// 발사 응답 패킷
	class FireAck : public IJsonSerializable
	{
	public:
		FireAck( void ) {}
		virtual ~FireAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 상대 발사 패킷
	class EnemyFireNotify : public IJsonSerializable
	{
	public:
		EnemyFireNotify( void ) {}
		virtual ~EnemyFireNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_fireType;
		int							_damage;
		float						_enemyPositionX;
		float						_enemyPositionY;
		float						_unitVecX;
		float						_unitVecY;
		float						_magnitude;
	};

	// 상대 발사 응답 패킷
	class EnemyFireAck : public IJsonSerializable
	{
	public:
		EnemyFireAck( void ) {}
		virtual ~EnemyFireAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 턴 종료 알림 패킷
	class TurnEndNotify : public IJsonSerializable
	{
	public:
		TurnEndNotify( void ) {}
		virtual ~TurnEndNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

	};

	// 턴 종료 응답 패킷
	class TurnEndAck : public IJsonSerializable
	{
	public:
		TurnEndAck( void ) {}
		virtual ~TurnEndAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

	};

	// 게임 종료 신청 패킷
	class GameSetRequest : public IJsonSerializable
	{
	public:
		GameSetRequest( void ) {}
		virtual ~GameSetRequest( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_winPlayerNum;
		int							_player1Hp;
		int							_player2Hp;
	};

	// 게임 종료 확인 패킷
	class GameSetAsk : public IJsonSerializable
	{
	public:
		GameSetAsk( void ) {}
		virtual ~GameSetAsk( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_winPlayerNum;
	};

	// 게임 종료 확인 응답 패킷
	class GameSetAnswer : public IJsonSerializable
	{
	public:
		GameSetAnswer( void ) {}
		virtual ~GameSetAnswer( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_answer;
		int							_player1Hp;
		int							_player2Hp;
	};

	// 게임 종료 알림 패킷
	class GameSetNotify : public IJsonSerializable
	{
	public:
		GameSetNotify( void ) {}
		virtual ~GameSetNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_isGameSet;
		int							_player1Hp;
		int							_player2Hp;
		int							_winPlayerNum;
	};

	// 게임 종료 응답 패킷
	class GameSetAck : public IJsonSerializable
	{
	public:
		GameSetAck( void ) {}
		virtual ~GameSetAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// 접속 닫기 패킷
	class CloseReq : public IJsonSerializable
	{
	public:
		CloseReq( void ) {}
		virtual ~CloseReq( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					_id;
		std::string					_token;
	};

	enum PacketId
	{
		ID_LoginReq			= 101,
		ID_LoginRes			= 102,
		ID_FastMatchReq			= 103,
		ID_FastMatchRes			= 104,
		ID_MatchCancelReq			= 105,
		ID_MatchCancelRes			= 106,
		ID_MatchSuccessNotify			= 107,
		ID_MatchSuccessAck			= 108,
		ID_GameStartNotify			= 109,
		ID_GameStartAck			= 110,
		ID_TurnStartNotify			= 111,
		ID_TurnStartAck			= 112,
		ID_EnemyTurnStartNotify			= 113,
		ID_EnemyTurnStartAck			= 114,
		ID_MoveNotify			= 115,
		ID_MoveAck			= 116,
		ID_EnemyMoveNotify			= 117,
		ID_EnemyMoveAck			= 118,
		ID_FireNotify			= 119,
		ID_FireAck			= 120,
		ID_EnemyFireNotify			= 121,
		ID_EnemyFireAck			= 122,
		ID_TurnEndNotify			= 123,
		ID_TurnEndAck			= 124,
		ID_GameSetRequest			= 125,
		ID_GameSetAsk			= 126,
		ID_GameSetAnswer			= 127,
		ID_GameSetNotify			= 128,
		ID_GameSetAck			= 129,
		ID_CloseReq			= 130,
	};
}
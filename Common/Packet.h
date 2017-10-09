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

	// C++ IOCP������ Unity C#�� ����ϱ� ���� ��Ŷ ���� �����Դϴ�.
	// �α��� ��û ��Ŷ
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

	// �α��� �亯 ��Ŷ
	class LoginRes : public IJsonSerializable
	{
	public:
		LoginRes( void ) {}
		virtual ~LoginRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ���� ��ġ ��û ��Ŷ
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

	// ��ġ ��û �亯 ��Ŷ
	class FastMatchRes : public IJsonSerializable
	{
	public:
		FastMatchRes( void ) {}
		virtual ~FastMatchRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ��ġ ��� ��Ŷ
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

	// ��ġ ��� �亯 ��Ŷ
	class MatchCancelRes : public IJsonSerializable
	{
	public:
		MatchCancelRes( void ) {}
		virtual ~MatchCancelRes( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ��Ī ���� �˸� ��Ŷ
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

	// ��Ī ���� ���� ��Ŷ
	class MatchSuccessAck : public IJsonSerializable
	{
	public:
		MatchSuccessAck( void ) {}
		virtual ~MatchSuccessAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ���� ���� �˸� ��Ŷ
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

	// ���� ���� ���� ��Ŷ
	class GameStartAck : public IJsonSerializable
	{
	public:
		GameStartAck( void ) {}
		virtual ~GameStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// �� �� ���� ��Ŷ
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

	// �� �� ���� ���� ��Ŷ
	class TurnStartAck : public IJsonSerializable
	{
	public:
		TurnStartAck( void ) {}
		virtual ~TurnStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ��� �� ���� ��Ŷ
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

	// ��� �� ���� ���� ��Ŷ
	class EnemyTurnStartAck : public IJsonSerializable
	{
	public:
		EnemyTurnStartAck( void ) {}
		virtual ~EnemyTurnStartAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ������ ���� ��Ŷ
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

	// ������ ���� ��Ŷ
	class MoveAck : public IJsonSerializable
	{
	public:
		MoveAck( void ) {}
		virtual ~MoveAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ��� ������ �˸� ��Ŷ
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

	// ��� ������ ���� ��Ŷ
	class EnemyMoveAck : public IJsonSerializable
	{
	public:
		EnemyMoveAck( void ) {}
		virtual ~EnemyMoveAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// �߻� ��Ŷ
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

	// �߻� ���� ��Ŷ
	class FireAck : public IJsonSerializable
	{
	public:
		FireAck( void ) {}
		virtual ~FireAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ��� �߻� ��Ŷ
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

	// ��� �߻� ���� ��Ŷ
	class EnemyFireAck : public IJsonSerializable
	{
	public:
		EnemyFireAck( void ) {}
		virtual ~EnemyFireAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// �� ���� �˸� ��Ŷ
	class TurnEndNotify : public IJsonSerializable
	{
	public:
		TurnEndNotify( void ) {}
		virtual ~TurnEndNotify( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

	};

	// �� ���� ���� ��Ŷ
	class TurnEndAck : public IJsonSerializable
	{
	public:
		TurnEndAck( void ) {}
		virtual ~TurnEndAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

	};

	// ���� ���� ��û ��Ŷ
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

	// ���� ���� Ȯ�� ��Ŷ
	class GameSetAsk : public IJsonSerializable
	{
	public:
		GameSetAsk( void ) {}
		virtual ~GameSetAsk( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_winPlayerNum;
	};

	// ���� ���� Ȯ�� ���� ��Ŷ
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

	// ���� ���� �˸� ��Ŷ
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

	// ���� ���� ���� ��Ŷ
	class GameSetAck : public IJsonSerializable
	{
	public:
		GameSetAck( void ) {}
		virtual ~GameSetAck( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							_result;
	};

	// ���� �ݱ� ��Ŷ
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
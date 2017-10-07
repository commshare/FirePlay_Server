#include "Packet.h"

namespace Packet
{
	bool CJsonSerializer::Serialize( IJsonSerializable* pObj, std::string& output )
	{
		if (pObj == NULL)
			return false;

		Json::Value serializeRoot;
		pObj->Serialize(serializeRoot);

		Json::StyledWriter writer;
		output = writer.write( serializeRoot );

		return true;
	}

	bool CJsonSerializer::Deserialize( IJsonSerializable* pObj, std::string& input )
	{
		if (pObj == NULL)
			return false;

		Json::Value deserializeRoot;
		Json::Reader reader;

		if ( !reader.parse(input, deserializeRoot) )
			return false;

		pObj->Deserialize(deserializeRoot);

		return true;
	}

	void LoginReq::Serialize( Json::Value& root )
	{
		root["_id"] = _id;
		root["_token"] = _token;
	}

	void LoginReq::Deserialize( Json::Value& root )
	{
		_id = root.get("_id", "").asString();
		_token = root.get("_token", "").asString();
	}

	void LoginRes::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void LoginRes::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void FastMatchReq::Serialize( Json::Value& root )
	{
		root["_type"] = _type;
		root["_id"] = _id;
		root["_token"] = _token;
	}

	void FastMatchReq::Deserialize( Json::Value& root )
	{
		_type = root.get("_type", 0).asInt();
		_id = root.get("_id", "").asString();
		_token = root.get("_token", "").asString();
	}

	void FastMatchRes::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void FastMatchRes::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void MatchCancelReq::Serialize( Json::Value& root )
	{
		root["_id"] = _id;
		root["_token"] = _token;
	}

	void MatchCancelReq::Deserialize( Json::Value& root )
	{
		_id = root.get("_id", "").asString();
		_token = root.get("_token", "").asString();
	}

	void MatchCancelRes::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void MatchCancelRes::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void MatchSuccessNotify::Serialize( Json::Value& root )
	{
		root["_gameNumber"] = _gameNumber;
		root["_enemyId"] = _enemyId;
		root["_enemyWins"] = _enemyWins;
		root["_enemyLoses"] = _enemyLoses;
		root["_enemyType"] = _enemyType;
	}

	void MatchSuccessNotify::Deserialize( Json::Value& root )
	{
		_gameNumber = root.get("_gameNumber", 0).asInt();
		_enemyId = root.get("_enemyId", "").asString();
		_enemyWins = root.get("_enemyWins", 0).asInt();
		_enemyLoses = root.get("_enemyLoses", 0).asInt();
		_enemyType = root.get("_enemyType", 0).asInt();
	}

	void MatchSuccessAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void MatchSuccessAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void GameStartNotify::Serialize( Json::Value& root )
	{
		root["_playerNumber"] = _playerNumber;
		root["_positionX"] = _positionX;
		root["_enemyPositionX"] = _enemyPositionX;
	}

	void GameStartNotify::Deserialize( Json::Value& root )
	{
		_playerNumber = root.get("_playerNumber", 0).asInt();
		_positionX = root.get("_positionX", 0.0).asDouble();
		_enemyPositionX = root.get("_enemyPositionX", 0.0).asDouble();
	}

	void GameStartAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void GameStartAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void TurnStartNotify::Serialize( Json::Value& root )
	{
		root["_windX"] = _windX;
		root["_windY"] = _windY;
	}

	void TurnStartNotify::Deserialize( Json::Value& root )
	{
		_windX = root.get("_windX", 0.0).asDouble();
		_windY = root.get("_windY", 0.0).asDouble();
	}

	void TurnStartAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void TurnStartAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void EnemyTurnStartNotify::Serialize( Json::Value& root )
	{
		root["_windX"] = _windX;
		root["_windY"] = _windY;
	}

	void EnemyTurnStartNotify::Deserialize( Json::Value& root )
	{
		_windX = root.get("_windX", 0.0).asDouble();
		_windY = root.get("_windY", 0.0).asDouble();
	}

	void EnemyTurnStartAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void EnemyTurnStartAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void MoveNotify::Serialize( Json::Value& root )
	{
		root["_enemyPositionX"] = _enemyPositionX;
	}

	void MoveNotify::Deserialize( Json::Value& root )
	{
		_enemyPositionX = root.get("_enemyPositionX", 0.0).asDouble();
	}

	void MoveAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void MoveAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void EnemyMoveNotify::Serialize( Json::Value& root )
	{
		root["_enemyPositionX"] = _enemyPositionX;
	}

	void EnemyMoveNotify::Deserialize( Json::Value& root )
	{
		_enemyPositionX = root.get("_enemyPositionX", 0.0).asDouble();
	}

	void EnemyMoveAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void EnemyMoveAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void FireNotify::Serialize( Json::Value& root )
	{
		root["_fireType"] = _fireType;
		root["_enemyPositionX"] = _enemyPositionX;
		root["_unitVecX"] = _unitVecX;
		root["_unitVecY"] = _unitVecY;
		root["_magnitude"] = _magnitude;
	}

	void FireNotify::Deserialize( Json::Value& root )
	{
		_fireType = root.get("_fireType", 0).asInt();
		_enemyPositionX = root.get("_enemyPositionX", 0.0).asDouble();
		_unitVecX = root.get("_unitVecX", 0.0).asDouble();
		_unitVecY = root.get("_unitVecY", 0.0).asDouble();
		_magnitude = root.get("_magnitude", 0.0).asDouble();
	}

	void FireAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void FireAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void EnemyFireNotify::Serialize( Json::Value& root )
	{
		root["_fireType"] = _fireType;
		root["_enemyPositionX"] = _enemyPositionX;
		root["_unitVecX"] = _unitVecX;
		root["_unitVecY"] = _unitVecY;
		root["_magnitude"] = _magnitude;
	}

	void EnemyFireNotify::Deserialize( Json::Value& root )
	{
		_fireType = root.get("_fireType", 0).asInt();
		_enemyPositionX = root.get("_enemyPositionX", 0.0).asDouble();
		_unitVecX = root.get("_unitVecX", 0.0).asDouble();
		_unitVecY = root.get("_unitVecY", 0.0).asDouble();
		_magnitude = root.get("_magnitude", 0.0).asDouble();
	}

	void EnemyFireAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void EnemyFireAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void TurnEndNotify::Serialize( Json::Value& root )
	{
	}

	void TurnEndNotify::Deserialize( Json::Value& root )
	{
	}

	void TurnEndAck::Serialize( Json::Value& root )
	{
	}

	void TurnEndAck::Deserialize( Json::Value& root )
	{
	}

	void GameSetNotify::Serialize( Json::Value& root )
	{
		root["_winPlayerNum"] = _winPlayerNum;
	}

	void GameSetNotify::Deserialize( Json::Value& root )
	{
		_winPlayerNum = root.get("_winPlayerNum", 0).asInt();
	}

	void GameSetAck::Serialize( Json::Value& root )
	{
		root["_result"] = _result;
	}

	void GameSetAck::Deserialize( Json::Value& root )
	{
		_result = root.get("_result", 0).asInt();
	}

	void CloseReq::Serialize( Json::Value& root )
	{
		root["_id"] = _id;
		root["_token"] = _token;
	}

	void CloseReq::Deserialize( Json::Value& root )
	{
		_id = root.get("_id", "").asString();
		_token = root.get("_token", "").asString();
	}

}
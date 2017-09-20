#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <random>

#include "../include/json/json.h"

#include "../Network/Network/PacketInfo.h"

#include "Define.h"
#include "Packet.h"

namespace FPCommon
{
	static class Util
	{
	public :

		// 패킷 정보를 Char Byte 형태에서 정의한 패킷 구조체로 Deserialize 하는 메소드.
		// Json 라이브러리의 CharReader 사용을 래핑.
		static void DeserializeFromCharByte(Packet::IJsonSerializable * outResult, std::shared_ptr<FPNetwork::PacketInfo> packetInfo)
		{
			Json::Value jsonValue;
			std::string errorMessage;

			auto readerBuilder = new Json::CharReaderBuilder();
			auto reader = readerBuilder->newCharReader();
			reader->parse(packetInfo->_body, packetInfo->_body + packetInfo->_bodySize, &jsonValue, &errorMessage);

			outResult->Deserialize(jsonValue);
		}

		// 패킷으로 온 Char Byte 형태의 데이터에서 원하는 데이터를 뽑아주는 메소드.
		template <class T>
		static T GetDataInCharByte(const char * data, int dataSize, std::string jsonItemName)
		{
			Json::Value jsonValue;
			std::string errorMessage;

			// TODO :: 이거 readerBuilder는 안만들어도 바로 newCharReader 선언해서 할당할 수 있나?
			auto readerBuilder = new Json::CharReaderBuilder();
			auto reader = readerBuilder->newCharReader();
			reader->parse(data, data + dataSize, &jsonValue, &errorMessage);

			// TODO :: int말고 다른 형식도 지원해야함.
			if (std::is_same<T, int>::value)
			{
				return jsonValue.get(jsonItemName, -1).asInt();
			}
		}

		// 패킷을 알맞은 형태로 SendQueue에 넣어주는 함수.
		static void PushToSendQueue(PacketQueue * sendQueue, Packet::PacketId packetId, const int sessionIdx, Packet::IJsonSerializable * packetToSend)
		{
			auto jsonBody = std::string();
			Packet::CJsonSerializer::Serialize(packetToSend, jsonBody);
			auto bodySize = static_cast<int>(strlen(jsonBody.c_str())) + 1;
			auto sendPacket = std::make_shared<FPNetwork::PacketInfo>();
			sendPacket->SetPacketInfo(packetId, sessionIdx, bodySize, jsonBody.c_str());

			sendQueue->Push(sendPacket);
		}

		// 의사 난수 생성기.
		static int GetRandomNumber(int min, int max)
		{
			std::mt19937 rng(time(0));
			std::uniform_int_distribution<int> dist(min, max);

			return dist(rng);
		}

	};
}

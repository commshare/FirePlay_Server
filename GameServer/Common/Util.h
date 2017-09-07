#pragma once
#include <memory>
#include <string>
#include <type_traits>

#include "../include/json/json.h"

#include "../Network/Network/PacketInfo.h"

#include "Define.h"
#include "Packet.h"

namespace FPCommon
{
	static class Util
	{
	public :

		// ��Ŷ ������ Char Byte ���¿��� ������ ��Ŷ ����ü�� Deserialize �ϴ� �޼ҵ�.
		// Json ���̺귯���� CharReader ����� ����.
		static void DeserializeFromCharByte(Packet::IJsonSerializable * outResult, std::shared_ptr<FPNetwork::PacketInfo> packetInfo)
		{
			Json::Value jsonValue;
			std::string errorMessage;

			auto readerBuilder = new Json::CharReaderBuilder();
			auto reader = readerBuilder->newCharReader();
			reader->parse(packetInfo->_body, packetInfo->_body + packetInfo->_bodySize, &jsonValue, &errorMessage);

			outResult->Deserialize(jsonValue);
		}

		// ��Ŷ���� �� Char Byte ������ �����Ϳ��� ���ϴ� �����͸� �̾��ִ� �޼ҵ�.
		template <class T>
		static T GetDataInCharByte(const char * data, int dataSize, std::string jsonItemName)
		{
			Json::Value jsonValue;
			std::string errorMessage;

			// TODO :: �̰� readerBuilder�� �ȸ��� �ٷ� newCharReader �����ؼ� �Ҵ��� �� �ֳ�?
			auto readerBuilder = new Json::CharReaderBuilder();
			auto reader = readerBuilder->newCharReader();
			reader->parse(data, data + dataSize, &jsonValue, &errorMessage);

			if (std::is_same<T, int>::value)
			{
				return jsonValue.get(jsonItemName, -1).asInt();
			}
		}
	};
}

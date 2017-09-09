#pragma once

namespace FPNetwork
{
#pragma pack(push, 1)

	struct PacketHeader
	{
		int _id;
		int _bodySize;
	};

	constexpr short packetHeaderSize = sizeof(PacketHeader);

	struct PacketInfo
	{
		PacketInfo() = default;
		// ERROR :: 패킷 인포에서 body를 삭제하면 메모리 이상한 접근뜸. ㅠㅠ
		// 근데 또 처리안하면 잘 돌아감. 처리를 해줘야 할 것 같은데 아닌가?
		// 내 생각엔 shared_ptr상태에서 queue의 pop이 호출되면서 두 번 삭제가 이루어지는 듯.
		~PacketInfo()
		{
			//if (_body != nullptr)
			//{
			//	delete[] _body;
			//	_body = nullptr;
			//}
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		char * _body = nullptr;
	};

#pragma pack(pop)
}

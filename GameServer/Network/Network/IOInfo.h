#pragma once
#include <winsock2.h>

namespace FPNetwork
{
	enum class IOInfoStatus : int
	{
		NONE = 0,
		READ = 1,
		WRITE = 2
	};

	struct IOInfo
	{
		OVERLAPPED Overlapped;
		WSABUF Wsabuf;
		IOInfoStatus Status = IOInfoStatus::NONE;
		int SessionTag = -1;
	};
}

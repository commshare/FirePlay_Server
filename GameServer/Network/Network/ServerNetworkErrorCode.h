#pragma once

namespace FPNetwork
{
	enum class NetworkErrorCode : short
	{
		None = 0,
		NotifyConnectSession = 2,
		NotifyCloseSession = 3
	};
}
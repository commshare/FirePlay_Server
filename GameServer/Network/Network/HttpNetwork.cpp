#include "HttpNetwork.h"

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"

namespace FPNetwork
{
	void HttpNetwork::Init(ConsoleLogger * logger, ServerConfig * config)
	{
		_logger = logger;

		// TODO :: DB 서버 주소를 config에서 가져오기.
	}
}

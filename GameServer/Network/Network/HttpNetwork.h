#pragma once
#include <string>

#include "happyhttp.h"

namespace FPCommon
{
	class ConsoleLogger;
	class ServerConfig;
}

namespace FPNetwork
{
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using ServerConfig = FPCommon::ServerConfig;

	/*
	 * DB 서버와의 Http통신을 전담하는 클래스.
	 */
	class HttpNetwork
	{
	public :

		HttpNetwork() {}
		~HttpNetwork() {}

		void Init(ConsoleLogger * logger, ServerConfig * config);

	private :


	private :

		ConsoleLogger * _logger;
		std::string     _dbServerUrl;
		int             _dbServerPort;

	};
}
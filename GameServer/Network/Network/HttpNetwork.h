#pragma once
#include <string>
#include <memory>

#include "happyhttp.h"

#include "../../Common/ConsoleLogger.h"
#include "../../Common/PacketQueue.h"
#include "../../Common/ErrorCode.h"

namespace FPCommon
{
	class ServerConfig;
	enum class LogType;
}

namespace FPNetwork
{
	using ConsoleLogger = FPCommon::ConsoleLogger;
	using PacketQueue   = FPCommon::PacketQueue;
	using ServerConfig  = FPCommon::ServerConfig;
	using LogType       = FPCommon::LogType;
	using ErrorCode     = FPCommon::ErrorCode;

	enum class ApiEnum : int
	{
		None = 0,
		TokenValidation = 1,
		ApiMaxNum = 2
	};

	// Http Network에 필요한 정보를 담아놓은 클래스.
	class HttpConfig
	{
	public :

		HttpConfig() = default;
		~HttpConfig() = default;

		std::string _dbServerUrl;
		int _dbServerPort;
		std::string _dbServerApi[static_cast<int>(ApiEnum::ApiMaxNum)];
	};

	/*
	 * DB 서버와의 Http통신을 전담하는 클래스.
	 */
	class HttpNetwork
	{
	public :

		HttpNetwork() {}
		~HttpNetwork() {}

		ErrorCode Init(ConsoleLogger * logger);

		// Token의 유효성을 검증하는 요청을 날리는 메소드.
		ErrorCode PostTokenValidationRequest(std::string id, std::string token);

	private :

		// DB 서버와의 Post 통신을 담당하는 메소드.
		std::string PostRequestToDBServer(std::string reqData, ApiEnum api);

		ErrorCode LoadHttpConfig();

	private :

		ConsoleLogger *             _logger;
		std::unique_ptr<HttpConfig> _config;

	};

	

}
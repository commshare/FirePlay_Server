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

	// Http Network�� �ʿ��� ������ ��Ƴ��� Ŭ����.
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
	 * DB �������� Http����� �����ϴ� Ŭ����.
	 */
	class HttpNetwork
	{
	public :

		HttpNetwork() {}
		~HttpNetwork() {}

		ErrorCode Init(ConsoleLogger * logger);

		// Token�� ��ȿ���� �����ϴ� ��û�� ������ �޼ҵ�.
		ErrorCode PostTokenValidationRequest(std::string id, std::string token);

	private :

		// DB �������� Post ����� ����ϴ� �޼ҵ�.
		std::string PostRequestToDBServer(std::string reqData, ApiEnum api);

		ErrorCode LoadHttpConfig();

	private :

		ConsoleLogger *             _logger;
		std::unique_ptr<HttpConfig> _config;

	};

	

}
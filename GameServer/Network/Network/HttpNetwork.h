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
	using PacketQueue = FPCommon::PacketQueue;
	using ServerConfig = FPCommon::ServerConfig;
	using LogType = FPCommon::LogType;
	using ErrorCode = FPCommon::ErrorCode;

	class HttpConfig
	{
	public :
		enum class ApiEnum : int
		{
			None = 0,
			TokenValidation = 1,
			ApiMaxNum = 2
		};

		HttpConfig() = default;
		~HttpConfig() = default;

		std::string _dbServerUrl;
		int _dbServerPort;
		std::string _dbServerApi[(int)ApiEnum::ApiMaxNum];
	};

	/*
	 * DB 서버와의 Http통신을 전담하는 클래스.
	 */
	class HttpNetwork
	{
	public :

		HttpNetwork() {}
		~HttpNetwork() {}

		ErrorCode Init(ConsoleLogger * logger, PacketQueue * recvQueue);

		std::string PostRequest(std::string reqData);

	private :

		ErrorCode LoadHttpConfig();

		//void OnBegin(const happyhttp::Response* r, void* userdata);

		//void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n);

		//void OnComplete(const happyhttp::Response* r, void* userdata);

	private :

		ConsoleLogger *             _logger;
		PacketQueue *               _recvQueue;
		std::unique_ptr<HttpConfig> _config;

	};
}
#include "HttpNetwork.h"
#include <fstream>
#include <memory>
#include <string>

#include "../../include/json/json.h"

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/json.hpp"
#include "../../Common/ErrorCode.h"
#include "../../Common/Util.h"

#include "PacketInfo.h"

namespace FPNetwork
{
	// WARN :: HappyHttp 라이브러리가 함수 형태를 이렇게 정의해 놓고 있어서 멤버 함수 콜백으로 사용하기가 껄적지근함.
	void OnBegin(const happyhttp::Response* r, void* userdata)
	{
	}

	void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
	{
		std::string &result = *(static_cast<std::string*>(userdata));
		result += std::string(reinterpret_cast<const char*>(data));
	}

	void OnComplete(const happyhttp::Response* r, void* userdata)
	{
	}

	using json = nlohmann::json;

	ErrorCode HttpNetwork::Init(ConsoleLogger * logger)
	{
		_logger = logger;

		if (LoadHttpConfig() != ErrorCode::None)
		{
			return ErrorCode::FailConfigLoad;
		}

		return ErrorCode::None;
	}

	ErrorCode HttpNetwork::PostTokenValidationRequest(std::string id, std::string token)
	{
		Json::Value tokenValidation;
		tokenValidation["Token"] = token;
		tokenValidation["UserId"] = id;

		auto userValidationString = PostRequestToDBServer(tokenValidation.toStyledString(), ApiEnum::TokenValidation);

		auto result = FPCommon::Util::GetDataInCharByte<int>(userValidationString.c_str(), strlen(userValidationString.c_str()), "Result");

		return static_cast<ErrorCode>(result);
	}

	std::string HttpNetwork::PostRequestToDBServer(std::string reqData, ApiEnum api)
	{
		const char* headers[] =
		{
			"Connection", "close",
			"Content-type", "application/json",
			"Accept", "text/plain",
			0
		};

		happyhttp::Connection conn(_config->_dbServerUrl.c_str(), _config->_dbServerPort);
		// 결과 값이 콜백 함수 호출시에 인자로 들어가도록 등록.
		std::string result;
		conn.setcallbacks(OnBegin, OnData, OnComplete, static_cast<void*>(&result));
		conn.request("POST",
			_config->_dbServerApi[static_cast<int>(api)].c_str(),
			headers,
			(const unsigned char*)reqData.c_str(),
			strlen(reqData.c_str()));

		while (conn.outstanding())
			conn.pump();

		return result;
	}

	ErrorCode HttpNetwork::LoadHttpConfig()
	{
		std::ifstream configFile("C:\\HttpConfig.json");
		if (configFile.fail())
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Config File Load Failed", __FUNCTION__);
			return ErrorCode::FailConfigLoad;
		}

		json configJson;
		configFile >> configJson;

		_config = std::make_unique<HttpConfig>();

		_config->_dbServerUrl = configJson["_dbServerUrl"].get<std::string>();
		_config->_dbServerPort = configJson["_dbServerPort"].get<int>();

		std::string api = "api";
		for (auto i = 1; i < static_cast<int>(ApiEnum::ApiMaxNum); ++i)
		{
			auto currentApi = api + std::to_string(i);
			_config->_dbServerApi[i] = configJson[currentApi].get<std::string>();
		}
	}
}

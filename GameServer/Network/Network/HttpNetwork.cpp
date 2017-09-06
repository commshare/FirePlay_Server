#include "HttpNetwork.h"
#include <fstream>
#include <memory>
#include <string>

#include "../../Common/ConsoleLogger.h"
#include "../../Common/Define.h"
#include "../../Common/json.hpp"
#include "../../Common/ErrorCode.h"

#include "PacketInfo.h"

namespace FPNetwork
{
	void OnBegin(const happyhttp::Response* r, void* userdata)
	{
	}

	void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
	{
		std::string &result = *(static_cast<std::string*>(userdata));
		result = std::string(reinterpret_cast<const char*>(data));
	}

	void OnComplete(const happyhttp::Response* r, void* userdata)
	{
	}

	using json = nlohmann::json;

	ErrorCode HttpNetwork::Init(ConsoleLogger * logger, PacketQueue * recvQueue)
	{
		_logger = logger;
		_recvQueue = recvQueue;

		LoadHttpConfig();
	}

	std::string HttpNetwork::PostRequest(std::string reqData)
	{
		std::string result;

		const char* headers[] =
		{
			"Connection", "close",
			"Content-type", "application/json",
			"Accept", "text/plain",
			0
		};

		happyhttp::Connection conn("localhost", 19000);
		conn.setcallbacks(OnBegin, OnData, OnComplete, static_cast<void*>(&result));
		conn.request("POST",
			"/Request/Login",
			headers,
			(const unsigned char*)reqData.c_str(),
			strlen(reqData.c_str()));

		while (conn.outstanding())
			conn.pump();

		return result;
	}

	ErrorCode HttpNetwork::LoadHttpConfig()
	{
		std::ifstream configFile("HttpConfig.json");
		if (configFile.fail())
		{
			_logger->Write(LogType::LOG_ERROR, "%s | Config File Load Failed", __FUNCTION__);
			return ErrorCode::FailConfigLoad;
		}

		json configJson;
		configFile >> configJson;

		_config = std::make_unique<HttpConfig>();

		_config->_dbServerUrl = configJson["_dbServerUrl"].get<std::string>();
		_config->_dbServerPort = std::stoi(configJson["_dbServerPort"].get<std::string>());

		std::string api = "api";
		for (auto i = 1; i < static_cast<int>(HttpConfig::ApiEnum::ApiMaxNum); ++i)
		{
			auto currentApi = api + std::to_string(i);
			_config->_dbServerApi[i] = configJson[currentApi].get<std::string>();
		}
	}

	//void HttpNetwork::OnBegin(const happyhttp::Response * r, void * userdata)
	//{
	//	_logger->Write(LogType::LOG_DEBUG, "%s | Begin HTTP POST Status(%d : %s)", __FUNCTION__, r->getstatus(), r->getreason());
	//}

	//void HttpNetwork::OnData(const happyhttp::Response * r, void * userdata, const unsigned char * data, int n)
	//{
	//	auto recvPacket = std::make_shared<PacketInfo>();

	//	// TODO :: 여기서부터 코딩 시작.

	//	
	//}

	//void HttpNetwork::OnComplete(const happyhttp::Response * r, void * userdata)
	//{
	//	_logger->Write(LogType::LOG_DEBUG, "%s | Complete HTTP POST Status(%d : %s)", __FUNCTION__, r->getstatus(), r->getreason());
	//}


}

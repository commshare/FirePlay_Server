#pragma once

#include "LogBase.h"

namespace FPCommon
{
	class ConsoleLogger : public LogBase
	{
	public:

		ConsoleLogger() {};
		virtual ~ConsoleLogger() {};

	protected:

		void error(const char * pText) override
		{
			consoleOutWithColor(ConsoleColor::RED, "[ERROR] ", pText);
		}

		void warn(const char * pText) override
		{
			consoleOutWithColor(ConsoleColor::YELLOW, "[WARN] ", pText);
		}

		void debug(const char * pText) override
		{
#ifdef _DEBUG
			consoleOutWithColor(ConsoleColor::WHITE, "[DEBUG] ", pText);
#endif
		}

		void trace(const char * pText) override
		{
			consoleOutWithColor(ConsoleColor::BLUE, "[TRACE] ", pText);
		}

		void info(const char * pText) override
		{
			consoleOutWithColor(ConsoleColor::GREEN, "[INFO] ", pText);
		}

		void consoleOutWithColor(const ConsoleColor color, const char* logHead, const char * logText)
		{
			std::lock_guard<std::mutex> _writeLock(_lock);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
			std::cout << logText << std::endl;
		}
	};
}
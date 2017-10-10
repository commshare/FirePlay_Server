#include "Minidump.h"

#include <crtdbg.h>
#include <stdio.h>
#include <Windows.h>
#include <string>

MiniDump * MiniDump::_instance = nullptr;

MiniDump * MiniDump::GetInstance()
{
	if (_instance == nullptr)
	{
		_instance = new MiniDump();
	}

	return _instance;
}

void MiniDump::DeleteInstance()
{
	if (_instance != nullptr)
	{
		delete _instance;
		_instance = nullptr;
	}
}

MiniDump::MiniDump()
{
	::SetUnhandledExceptionFilter(execptionFilter);
	printf("* Dump filter setting complte!\n");
}

LONG MiniDump::execptionFilter(_EXCEPTION_POINTERS * exceptionInfo)
{
	_CrtMemDumpAllObjectsSince(NULL);

	HMODULE dumpDll = nullptr;
	dumpDll = ::LoadLibraryA("DBGHELP.DLL");
	if (!dumpDll) {
		printf("! DBGHelp.dll not loaded\n");
		return 0;
	}

	std::string dumpPatch;
	dumpPatch += ("GameServer");
	dumpPatch += ".dmp";

	HANDLE file = ::CreateFile(dumpPatch.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		printf("! dump file not making\n");
		return 0;
	}

	_MINIDUMP_EXCEPTION_INFORMATION info;
	info.ThreadId = ::GetCurrentThreadId();
	info.ExceptionPointers = exceptionInfo;
	info.ClientPointers = NULL;

	WRITEDUMP dumpFunc = (WRITEDUMP)::GetProcAddress(dumpDll, "MiniDumpWriteDump");
	if (dumpFunc(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &info, NULL, NULL) == FALSE) {
		printf("! dump file saving error\n");
		return 0;
	}
	::CloseHandle(file);

	return EXCEPTION_CONTINUE_SEARCH;
}

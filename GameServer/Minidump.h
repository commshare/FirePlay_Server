#pragma once
#include <Windows.h>
#include <DbgHelp.h>

typedef BOOL(WINAPI *WRITEDUMP)(
	_In_  HANDLE hProcess,
	_In_  DWORD ProcessId,
	_In_  HANDLE hFile,
	_In_  MINIDUMP_TYPE DumpType,
	_In_  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	_In_  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	_In_  PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

class MiniDump 
{
public:

	static MiniDump * GetInstance();
	void DeleteInstance();

	MiniDump();

	static LONG WINAPI execptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);

private :

	static MiniDump * _instance;
};

#include "StdAfx.h"
#include "D_WinSE.h"
#include "D_Timestamp.h"
#include "D_Dir.h"
#include "D_StringUtils.h"
#include <Dbghelp.h>

void se_exception::trans_func(unsigned int u, EXCEPTION_POINTERS* exp )
{
  throw se_exception(exp);
}

void se_exception::minidump(const std::string &threadName)
{
	wchar_t dmpFile[128];
	SYSTEMTIME st = D_Timestamp::now().toUTCSysTime();
	_snwprintf(dmpFile, 128, L"SurdocDump_%04d%02d%02d%02d%02d%02d.%s.dmp", 
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond, wide(threadName).c_str());
	D_Dir dir;
	this->minidumpToFile(dir.GetLogDir() + dmpFile);
}

void se_exception::minidumpToFile(const std::wstring &path)
{
  HANDLE hFile = CreateFileW( path.c_str(), GENERIC_READ | GENERIC_WRITE, 
    0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

	if (hFile == INVALID_HANDLE_VALUE)
	{
		D_Log::WriteLog(D_Log::LOG_NORMAL, L"failed to create dump file: %s", path.c_str());
		return;
	}

  MINIDUMP_EXCEPTION_INFORMATION info;
  info.ThreadId = GetCurrentThreadId();
  info.ExceptionPointers = exp_;
  info.ClientPointers = FALSE;

  MiniDumpWriteDump(
    GetCurrentProcess(),
    GetCurrentProcessId(),
    hFile,
    MiniDumpNormal,
    &info,
    NULL,
    NULL);
  ::FlushFileBuffers(hFile);
  ::CloseHandle(hFile);
}

const char* se_exception::name() const
{
  static char buf [11] = {'0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

  switch (code_)
  {
  case EXCEPTION_ACCESS_VIOLATION :			return "Access Violation";
  case EXCEPTION_DATATYPE_MISALIGNMENT :		return "Datatype Misalignment";
  case EXCEPTION_BREAKPOINT :					return "Breakpoint";
  case EXCEPTION_SINGLE_STEP :				return "Single Step";
  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED :		return "Array Bounds Exceeded";
  case EXCEPTION_FLT_DENORMAL_OPERAND :		return "Float Denormal Operand";
  case EXCEPTION_FLT_DIVIDE_BY_ZERO :			return "Float Divide by Zero";
  case EXCEPTION_FLT_INEXACT_RESULT :			return "Float Inexact Result";
  case EXCEPTION_FLT_INVALID_OPERATION :		return "Float Invalid Operation";
  case EXCEPTION_FLT_OVERFLOW :				return "Float Overflow";
  case EXCEPTION_FLT_STACK_CHECK :			return "Float Stack Check";
  case EXCEPTION_FLT_UNDERFLOW :				return "Float Underflow";
  case EXCEPTION_INT_DIVIDE_BY_ZERO :			return "Integer Divide by Zero";
  case EXCEPTION_INT_OVERFLOW :				return "Integer Overflow";
  case EXCEPTION_PRIV_INSTRUCTION :			return "Privileged Instruction";
  case EXCEPTION_IN_PAGE_ERROR :				return "In Page Error";
  case EXCEPTION_ILLEGAL_INSTRUCTION :		return "Illegal Instruction";
  case EXCEPTION_NONCONTINUABLE_EXCEPTION :	return "Noncontinuable Exception";
  case EXCEPTION_STACK_OVERFLOW :				return "Stack Overflow";
  case EXCEPTION_INVALID_DISPOSITION :		return "Invalid Disposition";
  case EXCEPTION_GUARD_PAGE :					return "Guard Page";
  case EXCEPTION_INVALID_HANDLE :				return "Invalid Handle";
  case 0xE06D7363 :							return "Microsoft C++ Exception";
  default :									return _itoa(code_, &buf[2], 16);
  };		
}

se_translator::se_translator()
{
	old_ = _set_se_translator(&se_exception::trans_func);
}

se_translator::~se_translator()
{
	_set_se_translator(old_);
}

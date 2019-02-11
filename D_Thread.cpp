#include "StdAfx.h"
#include "D_Thread.h"
#include "D_StringUtils.h"
#include "D_Log.h"
#include "D_Dir.h"
#include "D_WinSE.h"
#include <stdexcept>
#include <cassert>

namespace {
  const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack(pop)

}

void SetThreadName(DWORD dwThreadID, const char* threadName)   
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}


DWORD WINAPI GenericThreadEntry(void *param)
{
  D_Thread *t = static_cast<D_Thread*>(param);
  SetThreadName(-1, t->name().c_str());
  se_translator st;
  try {
    D_Log::WriteLog(D_Log::LOG_DEBUG, "thread %s[%5u:%p] start", t->name().c_str(), t->tid(), t->handle());
    int ret = t->runInThread();
    D_Log::WriteLog(D_Log::LOG_DEBUG, "thread %s[%5u:%p] end", t->name().c_str(), t->tid(), t->handle());
    return ret;
  } catch (std::exception &ex) {
    /*D_Log::WriteLog(D_Log::LOG_HIGH, "uncaught std exception in thread %s[%u]", t->name().c_str(), t->tid());
    D_Log::WriteLog(D_Log::LOG_HIGH, "%s", ex.what());*/
    TerminateProcess(GetCurrentProcess(), 0);
  } catch (se_exception &ex) {
   /* D_Log::WriteLog(D_Log::LOG_HIGH, "uncaught se_exception in thread %s[%u]", t->name().c_str(), t->tid());
    D_Log::WriteLog(D_Log::LOG_HIGH, "addr: 0x%p, code: %u, name: %s", ex.addr(), ex.code(), ex.name());*/
		ex.minidump(t->name());
    TerminateProcess(GetCurrentProcess(), 0);
  } catch(...) {
    //D_Log::WriteLog(D_Log::LOG_HIGH, "uncaught exception in thread %s[%u]", t->name().c_str(), t->tid());
    TerminateProcess(GetCurrentProcess(), 0);
  }
  return 0;
}

D_Thread::~D_Thread(void)
{
  assert (thread_ != NULL);
  join();
  delete run_;
  ::CloseHandle(thread_);
  thread_ = NULL;
  run_ = NULL;
}

bool D_Thread::isActive()
{
  DWORD exitCode = 0;
  DWORD ret = ::GetExitCodeThread(thread_, &exitCode);
  return exitCode == STILL_ACTIVE;
}

bool D_Thread::join()
{
  //DEFINE_FUNC_TIMER;
  DWORD ret = ::WaitForSingleObject(thread_, INFINITE);
  if (ret == WAIT_OBJECT_0)
  {
    return true;
  } else {
    D_Log::WriteLog(D_Log::LOG_DEBUG, "join thread error: %s (return code: %u, errocode: %u)", 
      name_.c_str(), ret, ::GetLastError());  
    return false;
  }
}

bool D_Thread::join(long milliseconds)
{
  //DEFINE_FUNC_TIMER;
  DWORD ret = ::WaitForSingleObject(thread_, milliseconds);
  switch (ret) 
  {
  case WAIT_OBJECT_0:
    return true;
  case WAIT_TIMEOUT:
    return false;
  default:
    D_Log::WriteLog(D_Log::LOG_DEBUG, "join thread error: %s (return code: %u, errocode: %u)", 
      name_.c_str(), ret, ::GetLastError());  
    return false;
  }
}

int D_Thread::runInThread()
{
  return run_->run();
}
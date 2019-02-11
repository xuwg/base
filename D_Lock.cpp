#include "StdAfx.h"
#include "D_Lock.h"

#include <cassert>

D_Lock g_Lock;

D_Lock::D_Lock(void)
{
	InitializeCriticalSection ( &m_CriticalSection );
}

D_Lock::~D_Lock(void)
{
	DeleteCriticalSection ( &m_CriticalSection );
}
void D_Lock::Lock ()
{
	EnterCriticalSection ( &m_CriticalSection );
}
void D_Lock::UnLock ()
{
	LeaveCriticalSection ( &m_CriticalSection );
}
bool D_Lock::IsExist ()
{
	if (::TryEnterCriticalSection (&m_CriticalSection)==0)
	{
		return false;
	}
	return true;
}

void D_Lock::AssertHold() {
#if defined(DEBUG) || defined(_DEBUG)
	assert((DWORD)m_CriticalSection.OwningThread == ::GetCurrentThreadId());
#endif
}
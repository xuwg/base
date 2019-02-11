#include "StdAfx.h"
#include "D_Mutex.h"
//1:��ʱ���û����� 
//2:֮������ٽ���
D_Mutex::D_Mutex(void)
{
	m_hMutex = CreateMutex(NULL,FALSE, NULL);
}

D_Mutex::~D_Mutex(void)
{
	CloseHandle( m_hMutex );
}

bool D_Mutex::Lock (const wchar_t * _wTmp)
{
// 	TRACE("\r\nMutexData ");
// 	TRACE(_wTmp);
// 	TRACE("\r\n");
	D_Log::WriteLog(D_Log::LOG_DEBUG,_wTmp);
	if ( WAIT_FAILED == WaitForSingleObject( m_hMutex, INFINITE ) )
	{
		return false;
	} 
	return true;
}
bool D_Mutex::Lock ()
{
	if ( WAIT_FAILED == WaitForSingleObject( m_hMutex, INFINITE ) )
	{
		return false;
	} 
	return true;
}

bool D_Mutex::UnLock (const wchar_t * _wTmp)
{
// 	TRACE("\r\nMutexData ");
// 	TRACE(_wTmp);
// 	TRACE("\r\n");
	D_Log::WriteLog(D_Log::LOG_DEBUG,_wTmp);
	if ( !ReleaseMutex ( m_hMutex ) ) 
	{
		return false;
	}
	return true;
}
bool D_Mutex::UnLock ()
{
	if ( !ReleaseMutex ( m_hMutex ) ) 
	{
		return false;
	}
	return true;
}
bool D_Mutex::IsMutexExist(const wchar_t * _wTmp)
{
// 	TRACE(L"\r\nD_Mutex ");
// 	TRACE(_wTmp);
// 	TRACE("\r\n");
	D_Log::WriteLog(D_Log::LOG_DEBUG,_wTmp);
	if ( WAIT_OBJECT_0 == WaitForSingleObject( m_hMutex, 100 ) )
	{
		return true;
	}
	return false;
}
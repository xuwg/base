#include "StdAfx.h"
#include "D_CriticalSection.h"

CLS_CriticalSection * CLS_CriticalSection::m_pInstance = NULL;
CRITICAL_SECTION CLS_CriticalSection::m_CSection;
CLS_CriticalSection::CLS_CriticalSection(void)
{

}

CLS_CriticalSection::~CLS_CriticalSection(void)
{

}
CLS_CriticalSection& CLS_CriticalSection::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CLS_CriticalSection();
		m_pInstance->Initialize();
	}
	return *m_pInstance;
}//------------------------------------------------------------------
int CLS_CriticalSection::Initialize()
{
	if (m_pInstance)
	{
		::InitializeCriticalSection(&m_CSection);
	}
	return 1;
}
//------------------------------------------------------------------
int CLS_CriticalSection::UnInitialize()
{

	if (m_pInstance)
	{
		::DeleteCriticalSection(&m_CSection);
		delete m_pInstance;
		m_pInstance = NULL;
	}
	return 1;
}
bool CLS_CriticalSection::IsExist (string cstrTmp)
{
// 	TRACE("\r\n");
// 	TRACE(cstrTmp);
// 	TRACE("\r\n");

	D_Log::WriteLog(D_Log::LOG_DEBUG,cstrTmp.c_str());
	if (::TryEnterCriticalSection (&m_CSection)==0)
	{
		return false;
	}
	return true;
}
bool CLS_CriticalSection::Lock (string cstrTmp)
{
// 	TRACE("\r\n");
// 	TRACE(cstrTmp);
// 	TRACE("\r\n");
	D_Log::WriteLog(D_Log::LOG_DEBUG,cstrTmp.c_str());
	::EnterCriticalSection(&m_CSection);
	return true;
}

bool CLS_CriticalSection::UnLock (string cstrTmp)
{
// 	TRACE("\r\n");
// 	TRACE(cstrTmp);
// 	TRACE("\r\n");
	D_Log::WriteLog(D_Log::LOG_DEBUG,cstrTmp.c_str());
	::LeaveCriticalSection(&m_CSection);
	return true;
}

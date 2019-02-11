#include "stdafx.h"
#include "ThreadUtil.h"

CCriticalSection::CCriticalSection()
{
	InitializeCriticalSection(&cs_);
}

CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(&cs_);
}

void CCriticalSection::Enter()
{
	EnterCriticalSection(&cs_);
}

void CCriticalSection::Leave()
{
	LeaveCriticalSection(&cs_);
}
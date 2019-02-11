/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Lock.h
摘 要：安全锁
版 本：1.0
作 者：朱健
完成日期：2016-08-20
*********************************************************************/
#ifndef CS_LOCK_H
#define CS_LOCK_H
#pragma once




class  D_Lock
{
public:
	D_Lock(void);
public:
	~D_Lock(void);
	void Lock ();
	void UnLock ();
	bool IsExist ();
	void AssertHold();
private:
	CRITICAL_SECTION  m_CriticalSection;
};

class  D_ScopedLock
{
public:
  D_ScopedLock(D_Lock &lock): m_Lock(&lock) 
  { 
    m_Lock->Lock();
  }
  ~D_ScopedLock()
  { 
    m_Lock->UnLock();
  }
private:
  D_Lock *m_Lock;
};

#define DO_JOIN(a, b) a##b
#define JOIN(a, b) DO_JOIN(a, b)
#define SCOPE_LOCK(lock) D_ScopedLock JOIN(__lock_guard_, __LINE__)(lock);

#endif
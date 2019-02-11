/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Lock.h
ժ Ҫ����ȫ��
�� ����1.0
�� �ߣ��콡
������ڣ�2016-08-20
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
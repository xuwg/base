/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Mutex.h
ժ Ҫ��������->�������ݻ��洦��
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
*********************************************************************/
#ifndef MUTEX_H
#define MUTEX_H
#pragma once

class  D_Mutex
{
public:
	D_Mutex(void);
	virtual ~D_Mutex(void);
public:
	bool Lock (const wchar_t * _wTmp);  //����
	bool Lock ();  //����
	bool UnLock (const wchar_t * _wTmp);//����
	bool UnLock ();//����
	bool IsMutexExist(const wchar_t * _wTmp);
private:
	HANDLE    m_hMutex;//������
};
class  D_ScopedMutex
{
public:
	D_ScopedMutex(D_Mutex & lock):m_lock(&lock)
	{
		m_lock->Lock();
	}
	 ~D_ScopedMutex()
	 {
		 m_lock->UnLock();
	 }
private:
	D_Mutex * m_lock;
};
#endif


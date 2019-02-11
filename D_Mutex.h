/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Mutex.h
摘 要：互斥类->用作数据缓存处理
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
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
	bool Lock (const wchar_t * _wTmp);  //加锁
	bool Lock ();  //加锁
	bool UnLock (const wchar_t * _wTmp);//解锁
	bool UnLock ();//解锁
	bool IsMutexExist(const wchar_t * _wTmp);
private:
	HANDLE    m_hMutex;//互斥句柄
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


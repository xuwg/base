/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Event.h
摘 要：事件基类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "StoppingFlag.h"

class  D_Event: public IStoppingFlag
{
public:
	D_Event(bool manualReset = true, bool initialState = false);
	virtual ~D_Event(void);
	virtual bool IsStopping() { return Wait(0) == true; }

	bool Set();
	bool Reset();
	bool Wait();
	bool Wait(unsigned long milliSeconds);

	HANDLE handle() const { return m_Event; }

private:
	bool WaitVeryLongTime(unsigned long milliSeconds);

	HANDLE  m_Event;
};

#endif

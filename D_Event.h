/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Event.h
ժ Ҫ���¼�����
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
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

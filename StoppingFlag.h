#pragma once
#include <cassert>

/// ֹͣ��־������
struct IStoppingFlag
{
	IStoppingFlag() { }
	virtual ~IStoppingFlag() { }
	virtual bool IsStopping() = 0;
};

/// ����bool��ֹͣ��־
class BoolStoppingFlag: public IStoppingFlag
{
public:
	BoolStoppingFlag(void): stopping(false) { }
	virtual ~BoolStoppingFlag(void) { }

	virtual bool IsStopping() { return stopping; }
	bool Set() { assert(stopping == false); stopping = true; return true; }
	bool Reset() { assert(stopping == true); stopping = false; return true; }

private:
	volatile bool stopping;
};



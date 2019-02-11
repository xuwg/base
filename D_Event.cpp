#include "StdAfx.h"
#include "D_Event.h"



D_Event::D_Event(bool manualReset, bool initialState)
{
	m_Event = ::CreateEvent(NULL, manualReset, initialState, NULL);
}

D_Event::~D_Event(void)
{
	::CloseHandle(m_Event);
}

bool D_Event::Set()
{
	return (::SetEvent(m_Event) == TRUE);
}

bool D_Event::Reset()
{
	return (::ResetEvent(m_Event) == TRUE);
}

bool D_Event::Wait()
{
	switch(::WaitForSingleObject(m_Event, INFINITE))
	{
	case WAIT_OBJECT_0:
		return true;
	default:
		return false;
	}
}


bool D_Event::Wait(unsigned long milliSeconds)
{
	if (milliSeconds > 3600*1000)
		return WaitVeryLongTime(milliSeconds);

	switch(::WaitForSingleObject(m_Event, milliSeconds))
	{
	case WAIT_OBJECT_0:
		return true;
	case WAIT_TIMEOUT:
		return false;
	default:
		return false;
	}
}

bool D_Event::WaitVeryLongTime(unsigned long milliSeconds) {
	const unsigned long kMaxOneWaitInterval = 3600*1000;
	while (milliSeconds > 0) {
		unsigned long wait_interval = min(milliSeconds, kMaxOneWaitInterval);
		milliSeconds -= wait_interval;
		switch(::WaitForSingleObject(m_Event, wait_interval))
		{
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			continue;
		default:
			return false;
		}
	}

	return false;
}


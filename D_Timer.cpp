#include "StdAfx.h"
#include "D_Timer.h"
D_Timer::D_Timer(void)
{
}

D_Timer::~D_Timer(void)
{
}
void D_Timer::SetTimer ( HWND hWnd, unsigned int uIDEvent, unsigned int uElapse, void* lpTimerProc )
{
	::SetTimer ( hWnd, uIDEvent, uElapse, ( TIMERPROC ) lpTimerProc );
}
void D_Timer::KillTimer ( HWND hWnd, unsigned int uIDEvent )
{
	::KillTimer ( hWnd, uIDEvent );
}
void D_Timer::RunTimer ()
{
	MSG msg;
	while ( ::GetMessage ( &msg, NULL, 0, 0 ) )
	{
		if ( msg.message == WM_TIMER )
		{
			::DispatchMessage ( &msg );
		}
	}	
}
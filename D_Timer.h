#ifndef _D_TIMER_H__
#define _D_TIMER_H__

class  D_Timer
{
public:
	D_Timer(void);
public:
	~D_Timer(void);
public:
	static void SetTimer ( HWND hWnd, unsigned int uIDEvent, unsigned int uElapse, void* lpTimerProc );
	static void KillTimer ( HWND hWnd, unsigned int uIDEvent );
	static void RunTimer ();
};
#endif
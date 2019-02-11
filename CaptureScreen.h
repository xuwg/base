#ifndef CaptureScreen_h__
#define CaptureScreen_h__

#include <Windows.h>
/** 
*  @date        2018/12/05 17:56 
*  @brief       ������������ȡlpRect���򣬱���λͼ�ļ���filename,��Ҫ�������ͷŷ��ص�HBITMAP
*  @param[in]   filename
*  @param[in]   bitCount
*  @param[in]   lpRect
*  @param[in]   dfamplification �Ŵ���
*  @return      HBITMAP       
*  @remarks      
*  @see          
*/ 
HBITMAP ScreenCapture(LPSTR filename, WORD bitCount, const LPRECT lpRect, double dfamplification = 1.0);

#endif // CaptureScreen_h__


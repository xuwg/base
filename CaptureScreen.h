#ifndef CaptureScreen_h__
#define CaptureScreen_h__

#include <Windows.h>
/** 
*  @date        2018/12/05 17:56 
*  @brief       截屏函数，截取lpRect区域，保存位图文件到filename,需要调用者释放返回的HBITMAP
*  @param[in]   filename
*  @param[in]   bitCount
*  @param[in]   lpRect
*  @param[in]   dfamplification 放大倍数
*  @return      HBITMAP       
*  @remarks      
*  @see          
*/ 
HBITMAP ScreenCapture(LPSTR filename, WORD bitCount, const LPRECT lpRect, double dfamplification = 1.0);

#endif // CaptureScreen_h__


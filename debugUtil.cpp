#include <windows.h>
#include <Strsafe.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  函数名：	MzFormatAscString
//	功能  ：	将ASCII的内容格式化，方便输出日志信息等
//			 
//  参数  ：	[IN]  szFormat			格式化形式
//				[IN]  pArgs				参数
//  返回值：	PCHAR					
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *	MzFormatStringA(IN const char * pszFormat,IN va_list  pArgs)
{	
	char *	pszBuf	= NULL;
	DWORD	dwSize	= 512;

	for( ; ; )
	{

		pszBuf = (LPSTR)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,dwSize);
		if (!pszBuf)
		{
			break;
		}
		if (SUCCEEDED ( StringCchVPrintfA(pszBuf, dwSize, pszFormat, pArgs) ) )
		{
			break;
		}
		
		if( NULL != pszBuf)
		{
			HeapFree(GetProcessHeap(), 0, pszBuf);
		}

		dwSize *= 2;
		if(dwSize > 100*1024)
		{
			break;
		}
	}
	return pszBuf;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  函数名：	MzOutputDebugStringA
//	功能  ：	将ASCII的内容格式化，并输出到DbgView.exe
//			 
//  参数  ：	[IN]  szFormat			格式化形式
//				[IN]  ...				参数列表
//  返回值：	空					
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID	MzOutputDebugStringA( IN char * pszFormat,...)
{
	char *	pszString	= NULL;
	va_list pArgs		= NULL;

	va_start( pArgs, pszFormat );
	pszString = MzFormatStringA( pszFormat, pArgs );
	va_end( pArgs );
	if (pszString)
	{
		OutputDebugStringA(pszString);		

		if( NULL != pszString)
		{
			HeapFree(GetProcessHeap(), 0, pszString);
		}
	}
}
#include <windows.h>
#include <Strsafe.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ��������	MzFormatAscString
//	����  ��	��ASCII�����ݸ�ʽ�������������־��Ϣ��
//			 
//  ����  ��	[IN]  szFormat			��ʽ����ʽ
//				[IN]  pArgs				����
//  ����ֵ��	PCHAR					
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
//  ��������	MzOutputDebugStringA
//	����  ��	��ASCII�����ݸ�ʽ�����������DbgView.exe
//			 
//  ����  ��	[IN]  szFormat			��ʽ����ʽ
//				[IN]  ...				�����б�
//  ����ֵ��	��					
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
/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_MzApi.h
ժ Ҫ�����ýӿ�
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
*********************************************************************/
#ifndef	D_MZ_APL_DEF
#define	D_MZ_APL_DEF


PVOID	 MzHeapAlloc(IN DWORD dwSize);
VOID	 MzHeapFree(IN PVOID pBuf);

// ��һ���ַ��������ַ�
LPSTR lstrchr(LPCSTR lpStr, CHAR ch);
char  * Revstr(char *str, size_t len);
#endif


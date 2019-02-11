#include "StdAfx.h"
#include "D_MzApl.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  �������� MzHeapAlloc
//	����  �� ���� �����١�dwSize���ֽڵĿռ䣬�����õ����ڴ��ʼ��Ϊ0
//
//  ����  �� [IN]  dwSize			������ֽ���
//
//  ����ֵ�� PVOID					����Ķѿռ��ַ
//
//  ע�⣺	Ҫ�õ���ȷ�Ĵ�С����Ҫʹ�� ��HeapSize��
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PVOID	MzHeapAlloc(IN DWORD dwSize)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  �������� MzHeapFree
//	����  �� �ͷŶ��ڴ棬�����ڴ�ָ������ΪNULL
//
//  ����  �� [IN]  pBuf				�ڴ��ַ�ĵ�ַ
//
//  ����ֵ�� NULL					�޷���ֵ
//
//  ע�⣺	������ǡ���ַ�ĵ�ַ����Ϊ��ֱ�ӽ�����ڴ��ָ������ΪNULL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID	MzHeapFree(IN PVOID pBuf)
{
    if (NULL != pBuf)
    {
        HeapFree(GetProcessHeap(), 0, pBuf);
    }
}

// ��һ���ַ��������ַ�
LPSTR lstrchr(LPCSTR lpStr, CHAR ch)
{
    int i, len = lstrlenA(lpStr);

    if (lpStr == NULL)
        return NULL;

    for (i = 0; i < len; i++)
    {
        if (lpStr[i] == ch)
            return (LPSTR)lpStr + i;
    }

    return NULL;
}


char *Revstr(char *str, size_t len)
{

    char    *start = str;
    char    *end = str + len - 1;
    char    ch;

    if (str != NULL)
    {
        while (start < end)
        {
            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return str;
}

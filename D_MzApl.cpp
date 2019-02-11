#include "StdAfx.h"
#include "D_MzApl.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  函数名： MzHeapAlloc
//	功能  ： 分配 【最少】dwSize个字节的空间，并将得到的内存初始化为0
//
//  参数  ： [IN]  dwSize			申请的字节数
//
//  返回值： PVOID					申请的堆空间地址
//
//  注意：	要得到正确的大小，需要使用 【HeapSize】
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PVOID	MzHeapAlloc(IN DWORD dwSize)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  函数名： MzHeapFree
//	功能  ： 释放堆内存，并将内存指针设置为NULL
//
//  参数  ： [IN]  pBuf				内存地址的地址
//
//  返回值： NULL					无返回值
//
//  注意：	传入的是【地址的地址】，为了直接将存放内存的指针重置为NULL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID	MzHeapFree(IN PVOID pBuf)
{
    if (NULL != pBuf)
    {
        HeapFree(GetProcessHeap(), 0, pBuf);
    }
}

// 从一个字符串中找字符
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

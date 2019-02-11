/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_MzApi.h
摘 要：常用接口
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef	D_MZ_APL_DEF
#define	D_MZ_APL_DEF


PVOID	 MzHeapAlloc(IN DWORD dwSize);
VOID	 MzHeapFree(IN PVOID pBuf);

// 从一个字符串中找字符
LPSTR lstrchr(LPCSTR lpStr, CHAR ch);
char  * Revstr(char *str, size_t len);
#endif


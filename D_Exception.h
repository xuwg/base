/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Exception.h
ժ Ҫ���쳣��
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
*********************************************************************/
#ifndef _D_EXCEPTION_H__
#define _D_EXCEPTION_H__
#pragma once

class D_Exception
{
public:
	D_Exception ( unsigned int uErrorCode );
public:
	~D_Exception();
	 unsigned int GetErrorCode ();
	 string GetMessage ();
private:
	unsigned int m_uErrorCode;
	
};
#endif 
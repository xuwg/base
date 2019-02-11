/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Exception.h
摘 要：异常类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
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
/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：CLS_CriticalSection.h
摘 要：临界区：内存数据保护类
版 本：1.0
作 者：朱健修改
完成日期：2013-01-20
*********************************************************************/
#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H
#pragma once


class  CLS_CriticalSection
{
public:
	CLS_CriticalSection(void);
	virtual ~CLS_CriticalSection(void);
public:
	/************************************************************************
	Function：     GetInstance
	Description：  获得实体类指针
	Input：        
	Output：       
	Return：         
	Others：           
	************************************************************************/
	static CLS_CriticalSection& GetInstance();
	/************************************************************************
	Function：    UnInitialize
	Description： 初使系统资源
	Input：       
	Output：      
	Return：    <0 失败，> 0成功\0     
	Others：          
	************************************************************************/
	int Initialize();
	/************************************************************************
	Function：    UnInitialize
	Description： 释放系统资源
	Input：       
	Output：      
	Return：    <0 失败，> 0成功\0     
	Others：          
	************************************************************************/
	int UnInitialize();
public:
	bool Lock (string cstrTmp);  //加锁
	bool UnLock (string cstrTmp);//解锁
	bool IsExist (string cstrTmp);//解锁
private:
	HANDLE m_hCS;
	static CLS_CriticalSection * m_pInstance; //实例指针
	static CRITICAL_SECTION m_CSection; //保护 td 库安全 的信号量
};
#endif

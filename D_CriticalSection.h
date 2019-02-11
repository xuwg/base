/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�CLS_CriticalSection.h
ժ Ҫ���ٽ������ڴ����ݱ�����
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2013-01-20
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
	Function��     GetInstance
	Description��  ���ʵ����ָ��
	Input��        
	Output��       
	Return��         
	Others��           
	************************************************************************/
	static CLS_CriticalSection& GetInstance();
	/************************************************************************
	Function��    UnInitialize
	Description�� ��ʹϵͳ��Դ
	Input��       
	Output��      
	Return��    <0 ʧ�ܣ�> 0�ɹ�\0     
	Others��          
	************************************************************************/
	int Initialize();
	/************************************************************************
	Function��    UnInitialize
	Description�� �ͷ�ϵͳ��Դ
	Input��       
	Output��      
	Return��    <0 ʧ�ܣ�> 0�ɹ�\0     
	Others��          
	************************************************************************/
	int UnInitialize();
public:
	bool Lock (string cstrTmp);  //����
	bool UnLock (string cstrTmp);//����
	bool IsExist (string cstrTmp);//����
private:
	HANDLE m_hCS;
	static CLS_CriticalSection * m_pInstance; //ʵ��ָ��
	static CRITICAL_SECTION m_CSection; //���� td �ⰲȫ ���ź���
};
#endif

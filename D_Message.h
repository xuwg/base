/**********************************************************************
Copyright (c) 2016,���������������Ƽ����޹�˾
All rights reserved.
�ļ����ƣ�D_Message.h
ժ Ҫ����Ϣ��
�� ����1.0
�� �ߣ��콡�޸�
������ڣ�2016-08-20
*********************************************************************/
#ifndef D_MESSAGE_H
#define D_MESSAGE_H
#pragma once



class  D_Message
{
private:
    D_Message(void);
public:
    ~D_Message(void);
     static string GetMsg ( int nErrorCode );
     static wstring GetWMsg ( int nErrorCode );
};
#endif
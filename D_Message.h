/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Message.h
摘 要：消息类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
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
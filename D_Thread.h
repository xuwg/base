/**********************************************************************
Copyright (c) 2016,多啦宝（北京）科技有限公司
All rights reserved.
文件名称：D_Event.h
摘 要：线程基类
版 本：1.0
作 者：朱健修改
完成日期：2016-08-20
*********************************************************************/
#ifndef D_THREAD_H
#define D_THREAD_H

#pragma once

void  SetThreadName(DWORD dwThreadID, const char* threadName);

class Runnable
{
public:
  Runnable() { }
  virtual ~Runnable() { }
  virtual int run() = 0;
};

template <typename T>
class RunnableImpl: public Runnable
{
  typedef int(*FuncType)(T);
public:
  RunnableImpl(FuncType func, T data)
    : func_(func), data_(data) { }

  virtual ~RunnableImpl() { }
  int run() { return func_(data_); }

private:
  FuncType func_;
  T data_;
};

class  D_Thread
{
public:
  template <typename T>
  D_Thread(const std::string &name, int (*func)(T), T data);
  ~D_Thread(void);

  bool isActive();
  bool join(); 
  bool join(long milliseconds);
  const std::string &name() const { return name_; }
  DWORD tid() const { return tid_; }
  HANDLE handle() const { return thread_; }

public:
  /// you should never call this function
  int runInThread();

private:
  std::string name_;
  HANDLE thread_;
  DWORD  tid_;
  Runnable *run_;
};

 DWORD WINAPI  GenericThreadEntry(void *param);

template <typename T>
D_Thread::D_Thread(const std::string &name, int (*func)(T), T data)
:run_(new RunnableImpl<T>(func, data)),
name_(name)
{
  thread_ = ::CreateThread(NULL, 0, GenericThreadEntry, this, 0, &tid_);
}
#endif
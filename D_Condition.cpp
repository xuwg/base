#include "StdAfx.h"
#include "D_Condition.h"

D_Condition::D_Condition(void)
{
}

D_Condition::~D_Condition(void)
{
  //FIXME: 
  Broadcast();
}

void D_Condition::Wait(D_Lock &mutex)
{
  D_Event ev;
  {
    D_ScopedLock lock(mutex_);
    mutex.UnLock();
    waitQueue_.push_back(&ev);
  }
  ev.Wait();
  mutex.Lock();
}

void D_Condition::Signal()
{
  D_ScopedLock lock(mutex_);
  if (waitQueue_.empty())
    return;

  D_Event *first = waitQueue_.front();
  waitQueue_.pop_front();
  first->Set();
}

void D_Condition::Broadcast()
{
  D_ScopedLock lock(mutex_);
  std::list<D_Event*> temp;

  temp.swap(waitQueue_);
  WaitQueue::iterator it = temp.begin(), itEnd = temp.end();
  for (; it != itEnd; ++it)
    (*it)->Set();
  temp.clear();
}

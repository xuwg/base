#pragma once

// #include <list>
// #include "D_Lock.h"
// #include "D_Event.h"
// #include "SurbasicExport.h"

class  D_Condition
{
public:
  D_Condition(void);
  ~D_Condition(void);

  void Wait(D_Lock &mutex);
  void Signal();
  void Broadcast();

private:
  typedef std::list<D_Event *> WaitQueue;

  WaitQueue  waitQueue_;
  D_Lock      mutex_;
};

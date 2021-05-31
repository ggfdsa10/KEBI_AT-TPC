#ifndef DUMMYTASK_HH
#define DUMMYTASK_HH

#include "KBTask.hh"
#include "TClonesArray.h"

class DUMMYTask : public KBTask
{ 
  public:
    DUMMYTask();
    virtual ~DUMMYTask() {};

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fChannelArray;

  ClassDef(DUMMYTask, 1)
};

#endif

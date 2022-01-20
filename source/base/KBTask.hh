#ifndef KBTASK_HH
#define KBTASK_HH

#include "KBGear.hh"
#include "TTask.h"
#include <string>
#include <iostream>
using namespace std;

class KBTask : public TTask, public KBGear
{
  public:
    KBTask();
    KBTask(const char* name, const char *title);
    virtual ~KBTask() {};

    virtual void Add(TTask *task);

    virtual void SetRank(Int_t rank);

    bool InitTask();
    bool InitTasks();
    virtual bool Init();

    bool EndOfRunTask();
    bool EndOfRunTasks();
    virtual bool EndOfRun();

    ClassDef(KBTask, 1)
};

#endif

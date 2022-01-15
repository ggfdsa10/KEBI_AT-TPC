#include "KBTask.hh"

#include <iostream>
using namespace std;

KBTask::KBTask()
:TTask()
{
}

KBTask::KBTask(const char* name, const char *title)
:TTask(name, title)
{
}

void KBTask::Add(TTask *task)
{
  TTask::Add(task);

  auto kbtask = (KBTask *) task;
  kbtask -> SetRank(fRank+1);
  kbtask -> SetPar(fPar);
}

bool KBTask::InitTask() 
{
  if (!fActive)
    return false;

  bool initialized = Init();
  if (!initialized)
    return false;

  return InitTasks();
}

bool KBTask::Init() 
{
  return true;
}

bool KBTask::InitTasks()
{
  TIter iter(GetListOfTasks());
  KBTask* task;

  while ( (task = dynamic_cast<KBTask*>(iter())) ) {
    kb_info << "Initializing " << task -> GetName() << "." << endl;
    if (task -> Init() == false) {
      kb_warning << "Initialization failed!" << endl;
      return false;
    }
  }

  return true;
}

bool KBTask::EndOfRunTask()
{
  if (!fActive)
    return false;

  bool endofrun = EndOfRun();
  if (!endofrun)
    return false;

  return EndOfRunTasks();
}

bool KBTask::EndOfRun()
{
  return true;
}

bool KBTask::EndOfRunTasks()
{
  TIter iter(GetListOfTasks());
  KBTask* task;

  while ( (task = dynamic_cast<KBTask*>(iter())) ) {
    kb_info << "EndOfRun " << task -> GetName() << "." << endl;
    if (task -> EndOfRun() == false) {
      kb_warning << "EndOfRun failed!" << endl;
      return false;
    }
  }

  return true;
}

void KBTask::SetRank(Int_t rank)
{
  fRank = rank;

  TIter iter(GetListOfTasks());
  KBTask* task;
  while ( (task = dynamic_cast<KBTask*>(iter())) )
    task -> SetRank(fRank+1);
}

#include "KBRun.hh"
#include "KBChannelHit.hh"
#include "DUMMYTask.hh"

ClassImp(DUMMYTask)

DUMMYTask::DUMMYTask()
:KBTask("DUMMYTask","")
{
}

bool DUMMYTask::Init()
{
  bool saveThisBranch = true;
  fChannelArray = new TClonesArray("KBChannelHit", 100);
  KBRun::GetRun() -> RegisterBranch("Channelhit", fChannelArray, saveThisBranch);

  return true;
}

void DUMMYTask::Exec(Option_t*)
{
  fChannelArray -> Clear("C");

  auto hit = (KBChannelHit *) fChannelArray -> ConstructedAt(0);
  hit -> SetID(0);
  hit -> SetTDC(0);
  hit -> SetADC(123.987);

  hit = (KBChannelHit *) fChannelArray -> ConstructedAt(1);
  hit -> SetID(1);
  hit -> SetTDC(1.1);
  hit -> SetADC(1000);

  hit = (KBChannelHit *) fChannelArray -> ConstructedAt(2);
  hit -> SetID(2);
  hit -> SetTDC(22);
  hit -> SetADC(8);

  cout << "  [" << this -> GetName() << "] Message from this task" << endl;
}

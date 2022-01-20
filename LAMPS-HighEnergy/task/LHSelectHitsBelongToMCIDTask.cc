#include "LHSelectHitsBelongToMCIDTask.hh"

#include "KBRun.hh"
#include "KBTpcHit.hh"

ClassImp(LHSelectHitsBelongToMCIDTask)

LHSelectHitsBelongToMCIDTask::LHSelectHitsBelongToMCIDTask(int id)
:KBTask("LHSelectHitsBelongToMCIDTask",""), fSelectedMCID(id)
{
}

bool LHSelectHitsBelongToMCIDTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fOriginalHitArray = (TClonesArray *) run -> GetBranch("Hit");
  fSelectedHitArray = new TClonesArray("KBTpcHit");
  fBranchName = Form("Hit_MC%d",fSelectedMCID);
  run -> RegisterBranch(fBranchName, fSelectedHitArray, true);

  return true;
}

void LHSelectHitsBelongToMCIDTask::Exec(Option_t*)
{
  fSelectedHitArray -> Clear("C");

  auto numOriginalHits = fOriginalHitArray -> GetEntries();
  for (auto iHit=0; iHit<numOriginalHits; ++iHit)
  {
    auto originalHit = (KBTpcHit *) fOriginalHitArray -> At(iHit);
    if (fSelectedMCID == originalHit -> GetMCID()) {
      auto selectedHit = (KBTpcHit *) fSelectedHitArray -> ConstructedAt(fSelectedHitArray->GetEntriesFast());
      selectedHit -> CopyFrom(originalHit);
    }
  }

  kb_info << fBranchName << " " << fSelectedHitArray -> GetEntriesFast() << " " << fOriginalHitArray -> GetEntriesFast() << endl;

  return;
}

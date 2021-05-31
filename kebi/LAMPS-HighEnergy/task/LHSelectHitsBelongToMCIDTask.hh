#ifndef LHSELECTHITSBELONGTOMCIDTASK_HH
#define LHSELECTHITSBELONGTOMCIDTASK_HH

#include "KBTask.hh"
#include "TClonesArray.h"

class LHSelectHitsBelongToMCIDTask : public KBTask
{ 
  public:
    LHSelectHitsBelongToMCIDTask(int id);
    virtual ~LHSelectHitsBelongToMCIDTask() {}

    bool Init();
    void Exec(Option_t*);

    void SelectectMCID(int id) { fSelectedMCID = id; }

  private:
    TClonesArray* fOriginalHitArray;
    TClonesArray* fSelectedHitArray;

    bool fPersistency = true;

    TString fBranchName;
    int fSelectedMCID = -1;

  ClassDef(LHSelectHitsBelongToMCIDTask, 1)
};

#endif

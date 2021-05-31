#ifndef KBSINGLEHELIXTASK_HH
#define KBSINGLEHELIXTASK_HH

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "TClonesArray.h"
#include "KBVector3.hh"

class KBSingleHelixTask : public KBTask
{ 
  public:
    KBSingleHelixTask();
    virtual ~KBSingleHelixTask() {}

    bool Init();
    bool EndOfRun();
    void Exec(Option_t*);

    void SetTrackPersistency(bool val);

  private:
    TClonesArray* fHitArray;
    TClonesArray* fTrackArray;

    bool fPersistency = true;

    TTree *fTree = nullptr;
    Int_t fNumHits;
    Double_t fLength;
    Double_t fRMS, fRMSR, fRMST;

    KBVector3::Axis fReferenceAxis;

  ClassDef(KBSingleHelixTask, 1)
};

#endif

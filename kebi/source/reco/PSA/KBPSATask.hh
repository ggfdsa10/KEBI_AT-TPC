#ifndef KBPSATASK_HH
#define KBPSATASK_HH

#include "KBTask.hh"
#include "KBPSA.hh"
#include "KBPadPlane.hh"
#include "KBTpc.hh"

#include "TClonesArray.h"

class KBPSATask : public KBTask
{ 
  public:
    KBPSATask();
    virtual ~KBPSATask() {};

    bool Init();
    void Exec(Option_t*);

    void SetPSA(KBPSA *psa);

    void SetHitPersistency(bool persistence);

  private:
    TClonesArray* fPadArray;
    TClonesArray* fHitArray;

    bool fPersistency;

    KBTpc *fTpc;

    Int_t fNPlanes;
    Int_t fTbStart;
    Int_t fNTbs;
    Double_t fDriftVelocity;
    Double_t fTbTime;
    Double_t fADCThreshold;
    Double_t fTbLengthOffset = 0;

    KBPadPlane *fPadPlane[2];

    KBPSA *fPSA = nullptr;

  ClassDef(KBPSATask, 1)
};

#endif

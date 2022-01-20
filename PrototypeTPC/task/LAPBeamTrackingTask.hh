#ifndef LAMPSPROTOTYPEBEAMTRACKINGTASK_HH
#define LAMPSPROTOTYPEBEAMTRACKINGTASK_HH

#include "KBTask.hh"
#include "KBODRFitter.hh"
#include "TClonesArray.h"

class LAPBeamTrackingTask : public KBTask
{ 
  public:
    LAPBeamTrackingTask();
    virtual ~LAPBeamTrackingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetHitClusterPersistency(bool persistency) { fHitClusterPersistency = persistency; }
    void SetLinearTrackPersistency(bool persistency) { fLinearTrackPersistency = persistency; }
    void SetHitArrayPersistency(bool persistency) { fHitListPersistency = persistency; }

  private:
    TClonesArray* fHitArray;
    TClonesArray* fHitClusterArray;
    TClonesArray* fBeamTrackArray;

    bool fHitClusterPersistency = true;
    bool fLinearTrackPersistency = true;
    bool fHitListPersistency = true;

    Double_t fBeamTbRange1 = 0;
    Double_t fBeamTbRange2 = 511;
    Double_t fHitChargeHL = 3200;
    Int_t fNumSections = 4;
    Int_t fNumLayers[4] = {0};

  ClassDef(LAPBeamTrackingTask, 1)
};

#endif

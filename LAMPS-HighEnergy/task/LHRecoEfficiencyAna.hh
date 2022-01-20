#ifndef LHRECOEFFICIENCYANA_HH
#define LHRECOEFFICIENCYANA_HH

#include "KBTask.hh"
#include "KBMCTrack.hh"
#include "KBHelixTrack.hh"

#include "TClonesArray.h"

/**
 * Reconstructed-track Efficiency Analysis.
 * MC-ID propagated from MC tracks are use to calcuated efficiency.
 */

class LHRecoEfficiencyAna : public KBTask
{ 
  public:
    LHRecoEfficiencyAna();
    virtual ~LHRecoEfficiencyAna() {}

    bool Init();
    void Exec(Option_t*);

    void SetPersistency(bool val) { fPersistency = val; }

  private:
    TClonesArray* fMCTrackArray;
    TClonesArray* fRecoTrackArray;

    TClonesArray* fMCArray;
    TClonesArray* fRecoArray;

    Int_t *fMCIDIdx;

    bool fPersistency = true;

  ClassDef(LHRecoEfficiencyAna, 1)
};

#endif

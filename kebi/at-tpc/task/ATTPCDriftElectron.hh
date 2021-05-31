#ifndef ATTPCDRIFTELECTRON_HH
#define ATTPCDRIFTELECTRON_HH

#include "KBTask.hh"
#include "ATTPC.hh"
#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class ATTPCDriftElectron : public KBTask
{ 
  public:
    ATTPCDriftElectron();
    virtual ~ATTPCDriftElectron() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

  private:
    TClonesArray* fMCTrackArray;
    TClonesArray* fPadArray;
    ATTPC *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    bool fPersistency = true;

    Int_t fNPlanes = 0;
    Double_t fVelocityE = 0;
    Double_t fVelocityExB = 0;
    Double_t fLDiff = 0;
    Double_t fTDiff =0;
    Double_t fWvalue = 0;
    Int_t fNTbs = 0;
    Double_t fTBtime = 0;
  
    TH2D *fDiffusionFunction = nullptr;
    TF1 *fGainFunction = nullptr;
    Double_t fGainZeroRatio = 0;

  
  ClassDef(ATTPCDriftElectron, 1)
};

#endif

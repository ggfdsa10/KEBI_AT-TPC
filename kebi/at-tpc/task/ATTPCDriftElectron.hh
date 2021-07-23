#ifndef ATTPCDRIFTELECTRON_HH
#define ATTPCDRIFTELECTRON_HH

#include <time.h>
#include "KBTask.hh"
#include "ATTPC.hh"
#include "TF1.h"
#include "TRandom3.h"
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
    void GainDistribution();
    Double_t PolyaFunction(Double_t *x, Double_t *Par);

    TClonesArray* fMCTrackArray;
    TClonesArray* fPadArray;
    ATTPC *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    bool fPersistency = true;
    KBParameterContainer *par = nullptr;

    Int_t fNPlanes = 0;
    Double_t fGemVolt =0;
    Double_t fVelocityE = 0;
    Double_t fVelocityExB = 0;
    Double_t fLDiff = 0;
    Double_t fTDiff = 0;
    Double_t fWvalue = 0;
    Int_t fNTbs = 0;
    Double_t fTBtime = 0;
    Int_t fNumGEMLayer = 3;
    Double_t fGEMLayerTerm =0;

    TRandom3 *fRandom = nullptr;
    TF1 *fGainFunction = nullptr;
  
  ClassDef(ATTPCDriftElectron, 1)
};

#endif

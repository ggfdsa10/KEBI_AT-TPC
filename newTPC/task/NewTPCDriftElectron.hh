#ifndef NEWTPCDRIFTELECTRON_HH
#define NEWTPCDRIFTELECTRON_HH

#include <time.h>
#include "KBTask.hh"
#include "NewTPC.hh"
#include "TF1.h"
#include "TRandom3.h"
#include "TClonesArray.h"

class NewTPCDriftElectron : public KBTask
{ 
  public:
    NewTPCDriftElectron();
    virtual ~NewTPCDriftElectron() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

  private:
    void GainDistribution();
    Double_t PolyaFunction(Double_t *x, Double_t *Par);
    Double_t WvalueDistribution();
    Double_t TransverseDiffusion(Double_t length);

    TClonesArray* fMCTrackArray;
    TClonesArray* fPadArray;
    NewTPC *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    bool fPersistency = true;
    KBParameterContainer *par = nullptr;

    Int_t fNPlanes = 0;
    Double_t fGemVolt =0;
    Double_t fVelocityE = 0;
    Double_t fVelocityExB = 0;
    Double_t fLDiff = 0;
    Double_t fTDiff = 0;
    Double_t fBAt0DiffCoef2 =0;
    Double_t fWvalue = 0;
    Double_t fFanoFactor =0;
    Int_t fNTbs = 0;
    Double_t fTBtime = 0;
    Double_t fGEMLayerTerm =0;
    
    bool fNoise = false;
    bool fFastCalculate = false;

    TRandom3 *fRandom = nullptr;
    TF1 *fGainFunction = nullptr;
  
  ClassDef(NewTPCDriftElectron, 1)
};

#endif

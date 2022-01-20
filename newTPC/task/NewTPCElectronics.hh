#ifndef NEWTPCELECTRONICS_HH
#define NEWTPCELECTRONICS_HH

#include <time.h>
#include "KBTask.hh"
#include "LHTpc.hh"
#include "KBPulseGenerator.hh"
#include "TF1.h"
#include "TClonesArray.h"
#include "TRandom3.h"

class NewTPCElectronics : public KBTask
{ 
  public:
    NewTPCElectronics(Bool_t usePointChargeMC = true);
    virtual ~NewTPCElectronics() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;

    TRandom3 *fRandom = nullptr;  
    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t fEChargeToADC;
    Double_t fDynamicRange;
    Double_t fElectronCharge = 1.6021773349e-19; // [C]
    Double_t fADCMaxAmp = 4095; // Dynamic range maximum ratio

    TF1 *fPulseFunction;

    Bool_t fUsePointChargeMC = true;
    Bool_t fNoiseOn;

  ClassDef(NewTPCElectronics, 1)
};

#endif

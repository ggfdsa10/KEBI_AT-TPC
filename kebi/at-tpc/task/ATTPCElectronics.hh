#ifndef ATTPCELECTRONICS_HH
#define ATTPCELECTRONICS_HH

#include <time.h>
#include "KBTask.hh"
#include "LHTpc.hh"
#include "KBPulseGenerator.hh"
#include "TF1.h"
#include "TClonesArray.h"
#include "TRandom3.h"

class ATTPCElectronics : public KBTask
{ 
  public:
    ATTPCElectronics(Bool_t usePointChargeMC = true);
    virtual ~ATTPCElectronics() {}

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

  ClassDef(ATTPCElectronics, 1)
};

#endif
